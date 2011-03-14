#include "FACT.h"

/* * * * * * * * * * * * * * * * * * * * *
 * add_func:                             *
 *  - called by: '@'                     *
 *  - purpose: takes a function and      *
 *             adds the arguments and    *
 *             code block specified by   *
 *             the user.                 *
 *  - example:                           *
 *             @defunc example (def arg) *
 *               {                       *
 *                  return arg;          *
 *               }                       *
 *                                       *
 *             This sets example's args  *
 *             to "def arg" and its body *
 *             to "{ return arg }"       *
 *                                       *
 * * * * * * * * * * * * * * * * * * * * */
			
FACT_t
liven_func (func_t *scope, word_list expression)
{
  int       pos_args;
  int       pos_block;
  int       position;
  char   ** args_formatted;
  char   ** block_formatted;
  FACT_t    func;

  func = eval (scope, expression);

  if (func.type == ERROR_TYPE)
    return func;
  if (func.type != FUNCTION_TYPE)
    return errorman_throw_reg (scope, "cannot give body to non-function");
  if (func.f_point->locked)
    return errorman_throw_reg (scope, "function has been locked");

  expression.syntax += get_ip ();
  expression.lines  += get_ip ();

  if (expression.syntax[0] == NULL || tokcmp (expression.syntax[0], "("))
    return errorman_throw_reg (scope, "expected '(' after function");

  func.f_point->file_name      = scope->file_name;
  func.f_point->line           = scope->line;
  pos_args                     = get_exp_length (expression.syntax + 1, ')');
  args_formatted               = better_malloc (sizeof (char *) * pos_args);
  args_formatted[pos_args - 1] = NULL;
  position                     = pos_args - 1;
    
  while (position > 0)
    {
      position--;
      args_formatted[position] = add_newlines (expression.syntax[position + 1], expression.lines[position + 1]);
      scope->line += expression.lines[position + 1];
    }
    
  if (expression.syntax[pos_args] == NULL)
    return errorman_throw_reg (scope, "no body given");

  pos_block                    = get_exp_length_first (expression.syntax + pos_args, ';');
  block_formatted              = better_malloc (sizeof (char *) * pos_block);
  block_formatted[--pos_block] = NULL;
  position                     = pos_block;

  while (position > 0)
    {
      position--;
      block_formatted[position] = expression.syntax[position + pos_args + 1];
      block_formatted[position] = add_newlines (block_formatted[position], expression.lines[position + pos_args + 1]);
      scope->line              += expression.lines[position + pos_args + 1];
    }
  
  func.f_point->args = args_formatted;
  func.f_point->body = block_formatted;
  func.f_point->up   = scope;

  move_ip (pos_args + pos_block + 1);
  return func;
 }

 FACT_t
 prepare_function (func_t *scope, func_t *new_scope, word_list expression)
 {
   int          count;
   FACT_t       arg;
   FACT_t       evald;
   FACT_t       passed;
   word_list    arg_list;
   unsigned int ip;
   unsigned int ipe;
   unsigned int ipf;

   var_t *hold;
   var_t *temp;

   expression.syntax += get_ip ();
   expression.lines  += get_ip ();

   if (tokcmp (expression.syntax[0], "("))
     return errorman_throw_reg (scope, "expected '(' after function call");

   expression.syntax++;
   expression.lines++;

   ip = 1 + get_ip ();
   reset_ip ();
   evald = eval (scope, expression);  

   if (evald.type == ERROR_TYPE)
     return evald;

   if (evald.type != FUNCTION_TYPE)
     return errorman_throw_reg (scope, "cannot run a non-function");

   if (evald.f_point->args == NULL)
     return errorman_throw_reg (scope, "given function has no body");

   expression.syntax += get_ip ();
   expression.lines  += get_ip ();
   ip += get_ip ();

   arg_list              = make_word_list (evald.f_point->args, false);
   new_scope->file_name  = evald.f_point->file_name;
   new_scope->args       = evald.f_point->args;
   new_scope->body       = evald.f_point->body;
   new_scope->line       = evald.f_point->line;
   new_scope->up         = evald.f_point->up;
   new_scope->name       = evald.f_point->name;
   new_scope->extrn_func = evald.f_point->extrn_func;
   new_scope->variadic   = evald.f_point->variadic;

   if (arg_list.syntax[0] != NULL && tokcmp (expression.syntax[0], ","))
     return errorman_throw_reg (scope, combine_strs ("expected more arguments to function ", new_scope->name));

   ip++;
   reset_ip ();
   expression.syntax++;
   expression.lines++;

   if (arg_list.syntax[0] == NULL)
     {
       if (tokcmp (expression.syntax[-1], ")"))
	 return errorman_throw_reg (scope, "expected fewer arguments");
       set_ip (ip);
       return evald;
     }
   else if (tokcmp (expression.syntax[-1], ",")
	    && !tokcmp (arg_list.syntax[0], "->"))
     return evald;

   ipf = 0;
   ipe = 0;

   while (*arg_list.syntax != NULL)
     {
       if (!tokcmp (arg_list.syntax[0], "->")
	   || (!tokcmp (arg_list.syntax[0], ",") && !tokcmp (arg_list.syntax[1], "->")))
	 {
	   new_scope->line += (arg_list.lines[0] + arg_list.lines[1]
			       + (!tokcmp (arg_list.syntax[0], ",")) ? arg_list.lines[2] : 0);
	   ipe = 0;
	   while (tokcmp (expression.syntax[0], ")"))
	     {
	       struct _MIXED *go_through;

	       if (new_scope->variadic == NULL)
		 {
		   new_scope->variadic = better_malloc (sizeof (struct _MIXED));
		   go_through          = new_scope->variadic;
		 }
	       else
		 {
		   go_through->next = better_malloc (sizeof (struct _MIXED));
		   go_through       = go_through->next;
		 }
	       set_ip (ipe);
	       passed = eval (scope, expression);
	       ipe = get_ip ();

	       if (passed.type == ERROR_TYPE)
		 return passed;

	       go_through->type = passed.type;
	       go_through->next = NULL;

	       if (passed.type == VAR_TYPE)
		 go_through->var_p = passed.v_point;
	       else
		 go_through->func_p = passed.f_point;

	       expression.syntax += ipe;
	       expression.lines  += ipe;
	       ip += ipe;
	       
	       if (!tokcmp (expression.syntax[0], ","))
		 next_inst ();
	       ipe = 1;
	     }
	   break;
	 }

       set_ip (0);
       arg = eval (new_scope, arg_list);
       ipf = get_ip ();

       if (!tokcmp (expression.syntax[0], ")"))
	 return errorman_throw_reg (scope, "expected more arguments");

       set_ip (0);
       passed = eval (scope, expression);
       ipe  = get_ip ();
       ip  += ipe; 

       if (arg.type == ERROR_TYPE)
	 return arg;
       if (passed.type == ERROR_TYPE)
	 return passed;

       if (passed.type != arg.type)
	 return errorman_throw_reg (scope, "expected argument type does not match passed argument type");

       if (arg.type == VAR_TYPE)
	 {
	   hold                    = passed.v_point->next;
	   passed.v_point->next    = NULL;
	   temp                    = clone_var (passed.v_point, arg.v_point->name);
	   passed.v_point->next    = hold;
	   arg.v_point->array_up   = temp->array_up;

	   mpz_set (arg.v_point->array_size, temp->array_size);
	   mpc_set (&(arg.v_point->data), temp->data);
	 }
       else if (arg.type == FUNCTION_TYPE)
	 {
	   arg.f_point->line       = passed.f_point->line;
	   arg.f_point->file_name  = passed.f_point->file_name;
	   arg.f_point->args       = passed.f_point->args;
	   arg.f_point->body       = passed.f_point->body;
	   arg.f_point->usr_data   = passed.f_point->usr_data;
	   arg.f_point->extrn_func = passed.f_point->extrn_func;
	   arg.f_point->vars       = passed.f_point->vars;
	   arg.f_point->funcs      = passed.f_point->funcs;
	   arg.f_point->up         = passed.f_point->up;
	   arg.f_point->caller     = passed.f_point->caller;
	   arg.f_point->array_up   = passed.f_point->array_up;
	   arg.f_point->variadic   = passed.f_point->variadic;

	   mpz_set (arg.f_point->array_size, passed.f_point->array_size);
	 }

       arg_list.syntax   += ipf;
       arg_list.lines    += ipf;
       expression.syntax += ipe;
       expression.lines  += ipe;

       if (arg_list.syntax[0] == NULL)
	 {
	   if (tokcmp (expression.syntax[0], ")")
	       && tokcmp (expression.syntax[0], ",") == 0)
	     return errorman_throw_reg (scope, "expected fewer arguments");
	   else
	     break;
	 }
       else if (tokcmp (arg_list.syntax[0], ",") == 0)
	 {
	   if (tokcmp (expression.syntax[0], ",") != 0)
	     return errorman_throw_reg (scope, "expected more arguments");
	 }
       else
	 return errorman_throw_reg (new_scope, "syntax error in argument declaration");

       // There are more arguments, so we skip the comma.
       arg_list.syntax++;
       arg_list.lines++;
       expression.syntax++;
       expression.lines++;
       ip++;
     }

  set_ip (ip + 1);
  return evald;
}

FACT_t
new_scope (func_t *scope, word_list expression)
{
  /* new_scope - run a function and save its scope. This is a really
   * pretty simple function, I don't think that many will have trouble
   * understanding it. Oh wait, I forgot about the cast. It is a lot
   * of fun.
   */
  FACT_t        prepared;
  FACT_t        return_value;
  unsigned long ip;

  func_t *new_scope;

  new_scope = alloc_func ();
  prepared  = prepare_function (scope, new_scope, expression);

  if (prepared.type == ERROR_TYPE) 
    return prepared; /* Ha ha! "Return prepared!" */

  new_scope->caller = scope;

  if (new_scope->extrn_func != NULL)
    prepared = (((FACT_t (*)(func_t *)) new_scope->extrn_func) (new_scope));
  else
    {
      new_scope->line += strcount ('\n', prepared.f_point->body[0]);

      // Hold the instruction pointer for later and reset it. 
      ip = get_ip ();
      reset_ip ();
      // Run the procedure.
      prepared = procedure (new_scope, make_word_list (prepared.f_point->body + 1, false));
      // Restore the instruction pointer.
      set_ip (ip);
    }

  if (prepared.type == ERROR_TYPE)
    return prepared;
  
  return_value.f_point       = new_scope;
  return_value.type          = FUNCTION_TYPE;
  return_value.return_signal = false;
  return_value.break_signal  = false;

  return return_value;
}

FACT_t
run_func (func_t *scope, word_list expression_list)
{
  /* run_func - prepare and run a function without saving its
   * scope. Not much goes on in this function, so I tried to
   * make it as elegent as possible (although I'm not really
   * good at that).
   */

  FACT_t        return_value;
  FACT_t        prepared;
  unsigned long ip;

  func_t *new_scope;

  new_scope = alloc_func ();
  prepared  = prepare_function (scope, new_scope, expression_list);

  if (prepared.type == ERROR_TYPE)
    return prepared;

  new_scope->caller = scope;
  
  if (new_scope->extrn_func != NULL)
    return_value = (((FACT_t (*)(func_t *)) new_scope->extrn_func) (new_scope));
  else
    {
      new_scope->line += strcount ('\n', prepared.f_point->body[0]);
      ip = get_ip ();
      reset_ip ();
      return_value = procedure (new_scope, make_word_list (prepared.f_point->body + 1, false));
      set_ip (ip);      
    }

  return_value.return_signal = false;
  return_value.break_signal  = false;

  return return_value;
}

FACT_t
in_scope (func_t *scope, word_list expression)
{
  FACT_t new_scope;
  
  new_scope = eval (scope, expression);

  if (new_scope.type != FUNCTION_TYPE)
    return errorman_throw_reg (scope, "the argument to : must be a function");
  
  return eval (new_scope.f_point, expression);
}

FACT_t
lambda (func_t *scope, word_list expression)
{
  FACT_t return_value;

  return_value.type          = FUNCTION_TYPE;
  return_value.f_point       = alloc_func ();
  return_value.f_point->line = scope->line;
  return_value.f_point->name = "lambda";

  return return_value;
}

FACT_t
up (func_t *scope, word_list expression)
{
  FACT_t return_value;

  return_value.type = FUNCTION_TYPE;

  if (scope->up == NULL)
    return_value.f_point = scope;
  else
    return_value.f_point = scope->up;

  return return_value;
}

FACT_t
this (func_t *scope, word_list expression)
{
  FACT_t return_value;

  return_value.type    = FUNCTION_TYPE;
  return_value.f_point = scope;

  return return_value;
}

FACT_t
NULL_func (func_t *scope, word_list expression)
{
  FACT_t return_value;

  return_value.type    = FUNCTION_TYPE;
  return_value.f_point = alloc_func (); /* So that every value in the returned function is NULL */

  return return_value;
}
