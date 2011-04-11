/* This file is part of FACT.
 *
 * FACT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FACT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FACT. If not, see <http://www.gnu.org/licenses/>.
 */

#include "FACT.h"
			
FACT_t
liven_func (func_t *scope, syn_tree_t exp)
{
  // TODO: fix line numbers
  int    i;
  int    pos_args;
  int    pos_block;
  FACT_t func;

  char **args_formatted;
  char **block_formatted;

  func = eval (scope, exp);

  if (func.type == ERROR_TYPE)
    return func;
  if (func.type != FUNCTION_TYPE)
    FACT_throw (scope, "cannot give body to non-function", exp);
  if (func.f_point->locked)
    FACT_throw (scope, "function has been locked", exp);

  exp.syntax += get_ip ();
  func.f_point->file_name = scope->file_name;
  func.f_point->line = scope->line; 
  pos_args = get_exp_length (exp.syntax + 1, ')');
  args_formatted = FACT_malloc (sizeof (char *) * pos_args);
  args_formatted[pos_args - 1] = NULL;
  
  // Get the line of the function
  for (i = 0; i < (exp.syntax - exp.base); i++)
    func.f_point->line += exp.lines[i];
  func.f_point->lines = exp.lines + i;
  for (i = 0; i < pos_args - 1; i++)
    args_formatted[i] = exp.syntax[i + 1];
  
  pos_block = get_exp_length_first (exp.syntax + pos_args, ';');
  block_formatted = FACT_malloc (sizeof (char *) * pos_block);
  block_formatted[--pos_block] = NULL;

  for (i = 0; i < pos_block; i++)
    block_formatted[i] = exp.syntax[i + pos_args + 1];
  
  func.f_point->args = args_formatted;
  func.f_point->body = block_formatted;
  func.f_point->up = scope;
  move_ip (pos_args + pos_block + 1);
  return func;
 }

 FACT_t
 prepare_function (func_t *scope, func_t *new_scope, syn_tree_t expression)
 {
   // TODO: fix line numbers.
   int          count;
   FACT_t       arg;
   FACT_t       evald;
   FACT_t       passed;
   syn_tree_t   arg_list;
   unsigned int ip;
   unsigned int ipe;
   unsigned int ipf;

   var_t *hold;
   var_t *temp;

   expression.syntax += get_ip ();
   expression.syntax++;

   ip = 1 + get_ip ();
   reset_ip ();
   evald = eval (scope, expression);  

   if (evald.type == ERROR_TYPE)
     return evald;

   if (evald.type != FUNCTION_TYPE)
     FACT_throw (scope, "cannot run a non-function", expression);

   if (evald.f_point->args == NULL)
     FACT_throw (scope, "given function has no body", expression);

   expression.syntax += get_ip ();
   ip += get_ip ();

   arg_list = make_syn_tree (evald.f_point->args, evald.f_point->lines);
   new_scope->file_name = evald.f_point->file_name;
   new_scope->args = evald.f_point->args;
   new_scope->body = evald.f_point->body;
   new_scope->line = evald.f_point->line;
   new_scope->up = evald.f_point->up;
   new_scope->lines = evald.f_point->lines;
   new_scope->name = evald.f_point->name;
   new_scope->extrn_func = evald.f_point->extrn_func;
   new_scope->variadic = evald.f_point->variadic;

   if (arg_list.syntax[0] != NULL && tokcmp (expression.syntax[0], ","))
     FACT_throw (scope, combine_strs ("expected more arguments to function ", new_scope->name), expression);

   ip++;
   reset_ip ();
   expression.syntax++;

   if (arg_list.syntax[0] == NULL)
     {
       if (tokcmp (expression.syntax[-1], ")"))
	 FACT_throw (scope, "expected fewer arguments", expression);
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
           ipe = 0;
	   while (tokcmp (expression.syntax[0], ")"))
	     {
	       struct FACT_mixed *go_through;

	       if (new_scope->variadic == NULL)
		 {
		   new_scope->variadic = FACT_malloc (sizeof (struct FACT_mixed));
		   go_through = new_scope->variadic;
		 }
	       else
		 {
		   go_through->next = FACT_malloc (sizeof (struct FACT_mixed));
		   go_through = go_through->next;
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
	 FACT_throw (scope, "expected more arguments", expression);

       set_ip (0);
       passed = eval (scope, expression);
       ipe = get_ip ();
       ip += ipe; 

       if (arg.type == ERROR_TYPE)
	 return arg;
       if (passed.type == ERROR_TYPE)
	 return passed;

       if (passed.type != arg.type)
	 FACT_throw (scope, "expected argument type does not match passed argument type", expression);

       if (arg.type == VAR_TYPE)
	 {
	   hold = passed.v_point->next;
	   passed.v_point->next = NULL;
	   temp = clone_var (passed.v_point, arg.v_point->name);
	   passed.v_point->next = hold;
	   arg.v_point->array_up = temp->array_up;

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

       arg_list.syntax += ipf;
       expression.syntax += ipe;

       if (arg_list.syntax[0] == NULL)
	 {
	   if (tokcmp (expression.syntax[0], ")")
	       && tokcmp (expression.syntax[0], ",") == 0)
	     FACT_throw (scope, "expected fewer arguments", expression);
	   else
	     break;
	 }
       else if (tokcmp (arg_list.syntax[0], ",") == 0)
	 {
	   if (tokcmp (expression.syntax[0], ",") != 0)
	     FACT_throw (scope, "expected more arguments", expression);
	 }
       else
	 FACT_throw (new_scope, "syntax error in argument declaration", expression);

       // There are more arguments, so we skip the comma.
       arg_list.syntax++;
       expression.syntax++;
       ip++;
     }

  set_ip (ip + 1);
  return evald;
}

FACT_t
new_scope (func_t *scope, syn_tree_t exp)
{
  int    i, len; 
  FACT_t ret_val;
  FACT_t prepared;
  syn_tree_t func_body;
  unsigned long ip;

  func_t *new_scope;

  new_scope = alloc_func ();
  prepared = prepare_function (scope, new_scope, exp);

  if (prepared.type == ERROR_TYPE) 
    return prepared; 

  new_scope->caller = scope;

  if (new_scope->extrn_func != NULL)
    {
      prepared = (((FACT_t (*)(func_t *)) new_scope->extrn_func) (new_scope));
      if (ret_val.type == ERROR_TYPE)
        {
          ret_val.error.scope->file_name = scope->file_name;
          ret_val.error.scope->line = scope->line;
          FACT_throw (ret_val.error.scope, ret_val.error.description, exp);
        }
    }
  else
    {
      // Hold the instruction pointer for later and reset it. 
      ip = get_ip ();
      reset_ip ();
      func_body = make_syn_tree (prepared.f_point->body + 1, prepared.f_point->lines);
      len = count_until_NULL (prepared.f_point->args) + 2;
      for (i = 0; i <= len; i++)
        new_scope->line += func_body.lines[i];
      func_body.lines += i;
      func_body.base--;
      prepared = procedure (new_scope, func_body);
      // Restore the instruction pointer.
      set_ip (ip);
    }

  if (prepared.type == ERROR_TYPE)
    return prepared;
  
  ret_val.f_point = new_scope;
  ret_val.type = FUNCTION_TYPE;
  ret_val.return_signal = false;
  ret_val.break_signal = false;
  return ret_val;
}

FACT_t
run_func (func_t *scope, syn_tree_t exp)
{
  int    i, len;
  FACT_t ret_val;
  FACT_t prepared;
  syn_tree_t func_body;
  unsigned long ip;

  func_t *new_scope;

  new_scope = alloc_func ();
  prepared = prepare_function (scope, new_scope, exp);

  if (prepared.type == ERROR_TYPE)
    return prepared;

  new_scope->caller = scope;
  
  if (new_scope->extrn_func != NULL)
    {
      ret_val = (((FACT_t (*)(func_t *)) new_scope->extrn_func) (new_scope));
      if (ret_val.type == ERROR_TYPE)
        {
          ret_val.error.scope->file_name = scope->file_name;
          ret_val.error.scope->line = scope->line;
          FACT_throw (ret_val.error.scope, ret_val.error.description, exp);
        }
    }
  else
    {
      ip = get_ip ();
      reset_ip ();
      func_body = make_syn_tree (prepared.f_point->body + 1, prepared.f_point->lines);
      // We don't want to have to call this function for each iteration
      len = count_until_NULL (prepared.f_point->args) + 2;
      for (i = 0; i <= len; i++)
        new_scope->line += func_body.lines[i];
      func_body.lines += i;
      func_body.base--;
      ret_val = procedure (new_scope, func_body);
      set_ip (ip);      
    }

  ret_val.return_signal = false;
  ret_val.break_signal = false;
  return ret_val;
}

FACT_t
in_scope (func_t *scope, syn_tree_t exp)
{
  FACT_t new_scope;
  
  new_scope = eval (scope, exp);

  if (new_scope.type != FUNCTION_TYPE)
    FACT_throw (scope, "argument to : must be a function", exp);
  
  return eval (new_scope.f_point, exp);
}

FACT_t
lambda (func_t *scope, syn_tree_t expression)
{
  FACT_t ret_val;

  ret_val.type = FUNCTION_TYPE;
  ret_val.f_point = alloc_func ();
  ret_val.f_point->line = scope->line;
  ret_val.f_point->name = "lambda";

  return ret_val;
}

FACT_t
up (func_t *scope, syn_tree_t expression)
{
  FACT_t ret_val;

  ret_val.type = FUNCTION_TYPE;

  if (scope->up == NULL)
    ret_val.f_point = scope;
  else
    ret_val.f_point = scope->up;

  return ret_val;
}

FACT_t
this (func_t *scope, syn_tree_t expression)
{
  FACT_t ret_val;

  ret_val.type = FUNCTION_TYPE;
  ret_val.f_point = scope;

  return ret_val;
}

FACT_t
NULL_func (func_t *scope, syn_tree_t expression)
{
  FACT_t ret_val;

  ret_val.type = FUNCTION_TYPE;
  ret_val.f_point = alloc_func ();

  return ret_val;
}
