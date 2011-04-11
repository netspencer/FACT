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
if_statement (func_t *scope, syn_tree_t exp, bool *success)
{
  FACT_t return_value;
  FACT_t conditional;
  unsigned long ip;
  func_t *temp;

  temp = alloc_func ();
  temp->line = scope->line;
  temp->name = scope->name;
  temp->up = scope;
  mpz_init_set_ui (temp->array_size, 1);

  exp.syntax++;
  ip = get_ip ();
  set_ip (0);
  conditional = eval (temp, exp);

  if (conditional.type == ERROR_TYPE)
    {
      conditional.error.scope = scope;
      return conditional;
    }

  if (conditional.type == FUNCTION_TYPE)
    FACT_throw (scope, "if statement conditional must return a var", exp);
  
  if (mpc_cmp_si (conditional.v_point->data, 0) == 0)
    {
      return_value.v_point = alloc_var ();
      return_value.type = VAR_TYPE;
      return_value.return_signal = false;
      return_value.break_signal = false;
      *success = false;
      return return_value;
    }
  else
    *success = true;

  return_value = eval_expression (temp, exp);
  
  if (return_value.type == ERROR_TYPE)
    return_value.error.scope = scope;
  ip += get_ip ();
    
  return return_value;
}

FACT_t
error (func_t *scope, syn_tree_t exp, bool *success)
{
  FACT_t return_value;
  FACT_t conditional;
  unsigned long ip;
  var_t  *message;
  func_t *ERROR;
  func_t *temp;

  temp = alloc_func ();
  temp->line = scope->line;
  temp->name = scope->name;
  temp->up = scope;
  mpz_init_set_ui (temp->array_size, 1);

  exp.syntax++;
  ip = get_ip ();
  set_ip (0);
  conditional = eval (temp, exp);
  
  if (conditional.type != ERROR_TYPE)
    {
      return_value.v_point = alloc_var ();
      return_value.type = VAR_TYPE;
      return_value.return_signal = false;
      return_value.break_signal = false;
      *success = false;        
      return return_value;
    }
  else
    *success = true;
  
  /* This adds a local object called 'ERROR', which only
   * Can be accessed in the on_error's scope.
   *
   * ERROR:
   *  - locked => true
   *  - name => 'ERROR'
   *  - vars =>
   *  | - message => the error message of the caught exception.
   *  | | - locked => true
   *  - up => the scope of the caught exception.
   */
  
  ERROR = add_func (temp, "ERROR");
  ERROR->locked = true;
  ERROR->up = conditional.error.scope;
  message = add_var (ERROR, "message");
  message->array_up = string_to_array (conditional.error.description, "message");
  message->locked = true;
  mpz_set_si (message->array_size, strlen (conditional.error.description));
  
  return_value = eval_expression (temp, exp);

  if (return_value.type == ERROR_TYPE)
    return_value.error.scope = scope;
  ip += get_ip ();
    
  return return_value;
}

FACT_t
else_clause (func_t *scope, syn_tree_t expression)
{
  FACT_t return_value;
  func_t *temp;

  temp = alloc_func ();
  temp->line = scope->line;
  temp->name = scope->name;
  temp->up = scope;
  
  expression.syntax++;
  mpz_init_set_ui (temp->array_size, 1);

  reset_ip ();
  return_value = eval_expression (temp, expression);

  if (return_value.type == ERROR_TYPE)
    FACT_throw (scope, return_value.error.description, expression);

  return return_value;
}

FACT_t
while_loop (func_t *scope, syn_tree_t expression)
{
  FACT_t conditional_evald;
  FACT_t block_evald;
  func_t *temp;

  temp = alloc_func ();
  temp->line = scope->line;
  temp->name = scope->name;
  temp->up = scope;

  mpz_init_set_ui (temp->array_size, 1);

  expression.syntax++;
  block_evald.type = VAR_TYPE;
  block_evald.v_point = alloc_var ();
  block_evald.break_signal = false;

  for (;;)
    {
      reset_ip ();
      conditional_evald = eval (temp, expression);
      
      if (conditional_evald.type == ERROR_TYPE)
        return conditional_evald;
      if (conditional_evald.type == FUNCTION_TYPE)
        FACT_throw (scope, "while loop conditional must return a variable", expression);

      if (mpc_cmp_si (conditional_evald.v_point->data, 0) == 0)
        break;
      
      block_evald = eval_expression (temp, expression);

      if (block_evald.type == ERROR_TYPE || block_evald.return_signal || block_evald.break_signal)
	break;
    }
  return block_evald;
}

FACT_t
for_loop (func_t *scope, syn_tree_t expression)
{
  int    i;             
  mpc_t  one;
  FACT_t index_val;
  FACT_t lim_val;
  FACT_t block_evald;
  unsigned long long arr_pos;

  var_t  *var_scroller;
  func_t *func_scroller;
  func_t *temp;

  temp = alloc_func ();
  temp->line = scope->line;
  temp->name = scope->name;
  temp->up = scope;
  
  mpz_init_set_ui (temp->array_size, 1);

  expression.syntax++;
  reset_ip ();
  index_val = eval (temp, expression);

  if (index_val.type == ERROR_TYPE)
    return index_val;

  // Plus one to skip the comma.
  expression.syntax += get_ip () + 1;
  reset_ip ();
  lim_val = eval (temp, expression);

  if (lim_val.type == ERROR_TYPE)
    return lim_val;
  if (lim_val.type != index_val.type)
    FACT_throw (scope, "error in for loop; index type does not match destination type", expression);

  /* Since the parser would have rejected the code if a then didn't
   * follow the for statement, we can safely skip the token without
   * worrying about it.
   */
  expression.syntax += get_ip () + 1;
  mpc_init (&one);
  mpc_set_ui (&one, 1);

  arr_pos = 0;
  block_evald.type = VAR_TYPE;
  block_evald.v_point = alloc_var ();
  block_evald.break_signal = false;
  for (;;)
    {
      if (index_val.type == VAR_TYPE)
	{
	  if (mpz_cmp_ui (lim_val.v_point->array_size, 1) > 0)
	    {
	      if (mpz_cmp_ui (lim_val.v_point->array_size, arr_pos) <= 0)
		break;

	      for (var_scroller = lim_val.v_point->array_up, i = 0;
		   i < arr_pos; i++)
		var_scroller = var_scroller->next;

	      mpc_set (&(index_val.v_point->data), var_scroller->data);
	      mpz_set (index_val.v_point->array_size, var_scroller->array_size);
	      index_val.v_point->array_up = clone_var (var_scroller->array_up, index_val.v_point->name);
	    }
	  else if (arr_pos != 0)
	    {
              if (mpc_cmp (index_val.v_point->data, lim_val.v_point->data) > 0)
		mpc_sub (&(index_val.v_point->data), index_val.v_point->data, one);
	      else if (mpc_cmp (index_val.v_point->data, lim_val.v_point->data) < 0)
		mpc_add (&(index_val.v_point->data), index_val.v_point->data, one);
	      else
		break;
	    }
	}
      else
	{
	  if (mpz_cmp_ui (lim_val.f_point->array_size, 1) > 0)
	    {
	      if (mpz_cmp_ui (lim_val.v_point->array_size, arr_pos) <= 0)
		break;
              
	      for (func_scroller = lim_val.f_point->array_up, i = 1;
		   i < arr_pos; i++)
		func_scroller = func_scroller->next;

	      index_val.f_point->args = lim_val.f_point->args;
	      index_val.f_point->body = lim_val.f_point->body;
	      index_val.f_point->vars = lim_val.f_point->vars;
	      index_val.f_point->funcs = lim_val.f_point->funcs;
	      index_val.f_point->up = lim_val.f_point->up;
	      index_val.f_point->array_up = lim_val.f_point->array_up;
	      index_val.f_point->next = lim_val.f_point->next;

	      mpz_set (index_val.f_point->array_size, lim_val.f_point->array_size);
	    }
	  else
            FACT_throw (scope, "error in for loop; if the destination variable is a function, it must also be an array", expression);
	}

      reset_ip ();
      if (strcmp (expression.syntax[0], ";"))
	block_evald = eval_expression (temp, expression);

      if (block_evald.type == ERROR_TYPE || block_evald.return_signal == true || block_evald.break_signal == true)
	break;

      arr_pos++;
    }
  
  return block_evald;
}


