#include "common.h"

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
  int     * arg_lines;
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

  while (expression.move_forward[0])
    {
      expression.syntax++;
      expression.move_forward++;
      expression.lines++;
    }

  if (expression.syntax[0] == NULL || tokcmp (expression.syntax[0], "("))
    return errorman_throw_reg (scope, "expected '(' after function");

  func.f_point->line           = scope->line;
  pos_args                     = get_exp_length (expression.syntax + 1, ')');
  args_formatted               = better_malloc (sizeof (char *) * pos_args);
  arg_lines                    = better_malloc (sizeof (int   ) * pos_args); 
  args_formatted[pos_args - 1] = NULL;
  position                     = pos_args - 1;
    
  while (position > 0)
    {
      position--;
      args_formatted[position]              = expression.syntax[position + 1];
      arg_lines[position]                   = expression.lines[position + 1];
      scope->line                          += expression.lines[position + 1];
      expression.move_forward[position + 1] = true;
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
      block_formatted[position]                        = expression.syntax[position + pos_args + 1];
      // Seperateing these two out for clarity
      block_formatted[position]                        = add_newlines (block_formatted[position], expression.lines[position + pos_args + 1]);
      scope->line                                     += expression.lines[position + pos_args + 1];
      expression.move_forward[position + pos_args + 1] = true;
    }
  
  func.f_point->args       = args_formatted;
  func.f_point->arg_lines  = arg_lines;
  func.f_point->body       = block_formatted;
  func.f_point->up         = scope;
  
  for (position = 0; position < pos_args + pos_block; position++)
    expression.move_forward[position] = true;
  
  return func;
}

FACT_t
prepare_function (func_t *scope, func_t *new_scope, word_list expression)
{
  int         pos;
  int         count;
  var_t     * hold;
  var_t     * temp;
  FACT_t      evald;
  FACT_t      arg;
  FACT_t      passed;
  word_list   arg_list;

  if (tokcmp (expression.syntax[0], "("))
    return errorman_throw_reg (scope, "expected '(' after function call");
  else
    {
      expression.move_forward[0] = true;
      expression.move_forward++;
      expression.syntax++;
      expression.lines++;
    }
  
  evald = eval (scope, expression);

  if (evald.type == ERROR_TYPE)
    return evald;

  if (evald.type != FUNCTION_TYPE)
    return errorman_throw_reg (scope, "cannot run a non-function");

  if (evald.f_point->args == NULL)
    {
#ifdef DEBUG    
      printf (":%s\n", evald.f_point->name);
#endif
      return errorman_throw_reg (scope, "given function has no body");
    }

  while (expression.move_forward[0])
    {
      expression.syntax++;
      expression.move_forward++;
      expression.lines++;
    }
  
  arg_list.syntax       = copy (evald.f_point->args);
  arg_list.move_forward = better_malloc (sizeof (bool) * ((count = count_until_NULL (arg_list.syntax)) + 1));
  arg_list.lines        = evald.f_point->arg_lines;
  new_scope->line       = evald.f_point->line;
  new_scope->up         = evald.f_point->up;
  new_scope->name       = evald.f_point->name;
  new_scope->extrn_func = evald.f_point->extrn_func;

  if (arg_list.syntax[0] != NULL && tokcmp (expression.syntax[0], ","))
    return errorman_throw_reg (scope, "expected more arguments");
  else
    {
      expression.move_forward[0] = true;
      expression.move_forward++;
      expression.syntax++;
      expression.lines++;
    }

  if (arg_list.syntax[0] == NULL)
    {
      if (tokcmp (expression.syntax[-1], ")"))
	return errorman_throw_reg (scope, "expected fewer arguments");
      return evald;
    }
  
  for (pos = 0; arg_list.syntax[0] != NULL; pos++)
    {
      arg = eval (new_scope, arg_list);

      if (!tokcmp (expression.syntax[0], ")"))
	return errorman_throw_reg (scope, "expected more arguments");

      passed = eval (scope, expression);

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
	  arg.v_point->array_size = temp->array_size;
	  mpc_set (&(arg.v_point->data), temp->data);
	}
      else if (arg.type == FUNCTION_TYPE)
	{
	  arg.f_point->array_size = passed.f_point->array_size;
	  arg.f_point->args       = passed.f_point->args;
	  arg.f_point->body       = passed.f_point->body;
	  arg.f_point->vars       = passed.f_point->vars;
	  arg.f_point->funcs      = passed.f_point->funcs;
	  arg.f_point->array_up   = passed.f_point->array_up;
	  arg.f_point->up         = passed.f_point->up;
	  arg.f_point->extrn_func = passed.f_point->extrn_func;
	  arg.f_point->usr_data   = passed.f_point->usr_data;
	}

      while (arg_list.move_forward[0])
	{
	  arg_list.syntax++;
	  arg_list.move_forward++;
	  arg_list.lines++;
	}
      while (expression.move_forward[0])
	{
	  expression.syntax++;
	  expression.move_forward++;
	  expression.lines++;
	}
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
	  else
	    arg_list.move_forward[0] = expression.move_forward[0] = true;
	}
      else
	return errorman_throw_reg (new_scope, "syntax error in argument declaration");
	// return errorman_throw_reg (scope, "syntax error in argument declarations");
    }
  expression.move_forward[0] = true;
  return evald;
}

FACT_t
new_scope (func_t *scope, word_list expression)
{
  char   ** copy_body;
  func_t *  new_scope;
  FACT_t    prepared;
  FACT_t    return_value;

  new_scope = alloc_func ();
  
  prepared = prepare_function (scope, new_scope, expression);

  if (prepared.type == ERROR_TYPE) 
    return prepared; /* ha ha, that makes me chortle */

  if (new_scope->extrn_func != NULL)
    /* Well that is, um, oh wow. */ 
    prepared = (((FACT_t (*)(func_t *)) new_scope->extrn_func) (new_scope));
  else
    {
      copy_body = copy (prepared.f_point->body + 1);
      prepared = procedure (new_scope, make_word_list (copy_body));
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
  char   ** copied_body;
  func_t *  new_scope;
  FACT_t    return_value;
  FACT_t    prepared;

  new_scope = alloc_func ();
  prepared  = prepare_function (scope, new_scope, expression_list);

  if (prepared.type == ERROR_TYPE)
    return prepared; 
  
  copied_body = copy (prepared.f_point->body);

  if (new_scope->extrn_func != NULL)
    return_value = (((FACT_t (*)(func_t *)) new_scope->extrn_func) (new_scope));
  else
    return_value = eval_expression (new_scope, make_word_list (copied_body));

  return_value.return_signal = false;
  return_value.break_signal  = false;

  return return_value;
}

FACT_t
in_scope (func_t *scope, word_list expression)
{
  FACT_t new_scope;
  FACT_t to_return;

  new_scope = eval (scope, expression);

  if (new_scope.type != FUNCTION_TYPE)
    return errorman_throw_reg (scope, "the argument to : must be a function");
  
  to_return = eval (new_scope.f_point, expression);

  return to_return;
}

FACT_t
lambda (func_t *scope, word_list expression)
{
  FACT_t return_value;

  return_value.type          = FUNCTION_TYPE;
  return_value.f_point       = alloc_func ();
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
