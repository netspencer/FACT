#include "common.h"

FACT_t
invalid_if (func_t *scope, word_list expression)
{
  return errorman_throw_reg (scope, "invalid syntax, if statements must start at the beginning of the expression");
}

FACT_t
invalid_else (func_t *scope, word_list expression)
{
  return errorman_throw_reg (scope, "invalid syntax, else statements must follow if statements at the beginning of the expression");
}

FACT_t
invalid_while (func_t *scope, word_list expression)
{
  return errorman_throw_reg (scope, "invalid syntax, while loops must start at the beginning of the expression");
}

FACT_t
invalid_for (func_t *scope, word_list expression)
{
  return errorman_throw_reg (scope, "invalid syntax, for loops must start at the beginning of the expression");
}

FACT_t
if_statement (func_t *scope, word_list expression_list, bool *success)
{
  FACT_t return_value;
  FACT_t conditional;
  func_t temp_scope =
    {
      .name       = scope->name,
      .args       = NULL,
      .body       = NULL,
      .array_size = 1,
      .extrn_func = NULL,
      .vars       = NULL,
      .funcs      = NULL,
      .up         = scope,
      .array_up   = NULL,
      .next       = NULL,
    };
  
  *success = true;
  
  if (strcmp (expression_list.syntax[0], "(") != 0)
    return errorman_throw_reg (scope, "expected '(' after if statement");

  conditional = eval (&temp_scope, expression_list);

  if (conditional.type == ERROR_TYPE)
    {
      conditional.error.scope = scope;
      return conditional;
    }

  if (conditional.type == FUNCTION_TYPE)
    return errorman_throw_reg (scope, "if statement conditional must return a var_t");
  
  if (mpc_cmp_si (conditional.v_point->data, 0) == 0)
    {
      return_value.v_point = alloc_var ();
      return_value.type = VAR_TYPE;
      return_value.isret = false;
      return_value.break_signal = false;
      
      (*success) = false;  
      
      return return_value;
    }

  while (expression_list.move_forward[0])
    {
      expression_list.syntax++;
      expression_list.move_forward++;
    }
  
  return_value = expression (&temp_scope, expression_list.syntax);

  if (return_value.type == ERROR_TYPE)
    return_value.error.scope = scope;
    
  return return_value;
}

FACT_t
else_clause (func_t *scope, char **words)
{
  FACT_t return_value;  
  func_t temp_scope =
    {
      .name       = scope->name,
      .args       = NULL,
      .body       = NULL,
      .array_size = 1,
      .extrn_func = NULL,
      .vars       = NULL,
      .funcs      = NULL,
      .up         = scope,
      .array_up   = NULL,
      .next       = NULL,
    };

  return_value = expression (&temp_scope, words);

  if (return_value.type == ERROR_TYPE)
    return errorman_throw_reg (scope, return_value.error.description);

  return return_value;
}

FACT_t
while_loop (func_t *scope, char **words)
{
  int       pos_cond;
  int       pos;
  FACT_t    conditional_evald;
  FACT_t    block_evald;
  word_list conditional_exp;
  func_t    temp_scope =
    {
      .name       = scope->name,
      .args       = NULL,
      .body       = NULL,
      .array_size = 1,
      .extrn_func = NULL,
      .vars       = NULL,
      .funcs      = NULL,
      .up         = scope,
      .array_up   = NULL,
      .next       = NULL,
    };

  extern void set_array (bool *, int);

  if (words[0] == NULL || words[0][0] != '(')
    return errorman_throw_reg (scope, "expected '(' after while");

  pos_cond = get_exp_length (words + 1, ')');

  conditional_exp.syntax = words;
  conditional_exp.move_forward = better_malloc (sizeof (int) * (pos_cond));

  pos = pos_cond;

  if (words[pos_cond] == NULL)
    return errorman_throw_reg (scope, "syntax error in while loop");

  block_evald.type = VAR_TYPE;
  block_evald.v_point = alloc_var ();
  block_evald.break_signal = false;

  for (;;)
    {
      conditional_evald = eval (&temp_scope, conditional_exp);
      
      if (conditional_evald.type == ERROR_TYPE)
	return conditional_evald;
      
      if (conditional_evald.type == FUNCTION_TYPE)
	return errorman_throw_reg (scope, "while loop conditional must return a var_t");

      if (mpc_cmp_si (conditional_evald.v_point->data, 0) == 0)
        break;

      block_evald = expression (&temp_scope, words + pos_cond + 1);

      if (block_evald.type == ERROR_TYPE || block_evald.isret == true || block_evald.break_signal == true)
	break;

      set_array (conditional_exp.move_forward, pos_cond + 1);
    }

  return block_evald;
}

FACT_t
for_loop (func_t *scope, char **words)
{
  int         pos;
  int         count;
  int         arr_pos;  
  FACT_t      index_value;
  FACT_t      limit_value;
  FACT_t      block_evald;
  word_list   index_dest_exp;
  mpc_t       one;
  var_t     * var_t_scroller;
  func_t    * func_t_scroller;
  func_t      temp_scope =
    {
      .name       = scope->name,
      .args       = NULL,
      .body       = NULL,
      .array_size = 1,
      .extrn_func = NULL,
      .vars       = NULL,
      .funcs      = NULL,
      .up         = scope,
      .array_up   = NULL,
      .next       = NULL,
    };
  
  extern int count_until_NULL (char **);
  extern void set_array (bool *, int);

  index_dest_exp.syntax = words;
  index_dest_exp.move_forward = better_malloc (sizeof (int) *
					       ((count = count_until_NULL (words)) + 1));
    
  index_value = eval (&temp_scope, index_dest_exp);

  if (index_value.type == ERROR_TYPE)
    return index_value;

  while (index_dest_exp.move_forward[0])
    {
      index_dest_exp.syntax++;
      index_dest_exp.move_forward++;
    }

  if (strcmp (index_dest_exp.syntax[0], ","))
    return errorman_throw_reg (scope, "syntax error in for loop; missing ','");

  index_dest_exp.move_forward[0] = true;

  limit_value = eval (&temp_scope, index_dest_exp);

  if (limit_value.type == ERROR_TYPE)
    return limit_value;

  if (limit_value.type != index_value.type)
    return errorman_throw_reg (scope, "error in for loop; index type does not match destination type");

  while (index_dest_exp.move_forward[0])
    {
      index_dest_exp.syntax++;
      index_dest_exp.move_forward++;
    }
  
  if (strcmp (index_dest_exp.syntax[0], "then"))
    return errorman_throw_reg (scope, "syntax error in for loop; missing 'then'");

  mpc_init (&one);
  mpc_set_ui (&one, 1);

  arr_pos = 0;

  block_evald.type = VAR_TYPE;
  block_evald.v_point = alloc_var ();
  block_evald.break_signal = false;

  for (;;)
    {
      if (index_value.type == VAR_TYPE)
	{
	  if (limit_value.v_point->array_size > 1)
	    {
	      if (arr_pos >= limit_value.v_point->array_size)
		break;

	      for (var_t_scroller = limit_value.v_point->array_up, pos = 0;
		   pos < arr_pos; pos++)
		var_t_scroller = var_t_scroller->next;

	      index_value.v_point->array_size = var_t_scroller->array_size;
	      mpc_set (&(index_value.v_point->data), var_t_scroller->data);
	      index_value.v_point->array_up = clone_var_t (var_t_scroller->array_up, index_value.v_point->name);
	    }
	  else if (arr_pos != 0)
	    {
	      if (mpc_cmp (index_value.v_point->data, limit_value.v_point->data) > 0)
		mpc_sub (&(index_value.v_point->data), index_value.v_point->data, one);
	      else if (mpc_cmp (index_value.v_point->data, limit_value.v_point->data) < 0)
		mpc_add (&(index_value.v_point->data), index_value.v_point->data, one);
	      else
		break;
	    }
	}
      else
	{
	  if (limit_value.f_point->array_size > 1)
	    {
	      if (arr_pos >= limit_value.f_point->array_size)
		break;

	      for (func_t_scroller = limit_value.f_point->array_up, pos = 1;
		   pos < arr_pos; pos++)
		func_t_scroller = func_t_scroller->next;

	      index_value.f_point->args = limit_value.f_point->args;
	      index_value.f_point->body = limit_value.f_point->body;
	      index_value.f_point->array_size = limit_value.f_point->array_size;
	      index_value.f_point->vars = limit_value.f_point->vars;
	      index_value.f_point->funcs = limit_value.f_point->funcs;
	      index_value.f_point->up = limit_value.f_point->up;
	      index_value.f_point->array_up = limit_value.f_point->array_up;
	      index_value.f_point->next = limit_value.f_point->next;
	    }
	  else
	    return errorman_throw_reg (scope, "error in for loop; if the destination var_tiable is a function, it must also be an array");
	}

      if (strcmp (index_dest_exp.syntax[1], ";"))
	block_evald = expression (&temp_scope, index_dest_exp.syntax + 1);

      if (block_evald.type == ERROR_TYPE || block_evald.isret == true || block_evald.break_signal == true)
	break;

      arr_pos++;
    }

  return block_evald;
}


