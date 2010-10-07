#include "common.h"

a_type
invalid_if (func *scope, char **words)
{
  return errorman_throw_reg (scope, "invalid syntax, if statements must start at the beginning of the expression");
}

a_type
invalid_else (func *scope, char **words)
{
  return errorman_throw_reg (scope, "invalid syntax, else statements must follow if statements at the beginning of the expression");
}

a_type
invalid_while (func *scope, char **words)
{
  return errorman_throw_reg (scope, "invalid syntax, while loops must start at the beginning of the expression");
}

a_type
invalid_for (func *scope, char **words)
{
  return errorman_throw_reg (scope, "invalid syntax, for loops must start at the beginning of the expression");
}

a_type
if_statement (func *scope, char **words, bool *success)
{
  a_type return_value;
  a_type conditional;
  /*int pos;
    int count;*/

  func temp_scope =
    {
      "if_temp",
      NULL,
      NULL,
      1,
      NULL,
      NULL,
      scope,
      NULL,
      NULL
    };
  
  (*success) = true;
  
  if (strcmp (*words, "(") != 0)
    return errorman_throw_reg (scope, "expected '(' after if statement");

  conditional = eval (&temp_scope, words);

  if (conditional.type == ERROR_TYPE)
    {
      conditional.error.scope = scope;
      return conditional; //errorman_throw_reg (scope, conditional.error.description);
    }

  if (conditional.type == FUNCTION_TYPE)
    return errorman_throw_reg (scope, "if statement conditional must return a var");
  
  if (mpc_cmp_si (conditional.v_point->data, 0) == 0)
    {
      /*for (pos = 1, count = 0; words[pos] != NULL; pos++)
        {
          if (words[pos][0] == '{'
	      || words[pos][0] == '('
	      || words[pos][0] == '[')
            count++;
          else if (words[pos][0] == '}'
		   || words[pos][0] == ')'
		   || words[pos][0] == ']')
            count--;
          else if (strcmp (words[pos], "else") == 0 && count == 0)
            break;
        }

      if (count > 0)
	return errorman_throw_reg (scope, "syntax error in if statement");

      if (words[pos] == NULL || strcmp (words[pos], "else") != 0)
      { */
          return_value.v_point = alloc_var ();
	  return_value.type = VAR_TYPE;
	  return_value.isret = false;
	  return_value.break_signal = false;

	  (*success) = false;  
	  
          return return_value;
	  /*  }
      else
	{
	  return_value = expression (&temp_scope, (words + pos + 1));

	  if (return_value.type == ERROR_TYPE)
	    return errorman_throw_reg (scope, combine_strs ("error in if statement block; ",
							    return_value.error.description));

	  return return_value;
	  } */
    }
  
  return_value = expression (&temp_scope, (words + 1));

  if (return_value.type == ERROR_TYPE)
    return_value.error.scope = scope;//errorman_throw_reg (scope, return_value.error.description);
    
  return return_value;
}

a_type
else_clause (func *scope, char **words)
{
  a_type return_value;
  
  func temp_scope =
    {
      "if_temp",
      NULL,
      NULL,
      1,
      NULL,
      NULL,
      scope,
      NULL,
      NULL
    };

  return_value = expression (&temp_scope, words);

  if (return_value.type == ERROR_TYPE)
    return errorman_throw_reg (scope, return_value.error.description);

  return return_value;
}

a_type
while_loop (func *scope, char **words)
{
  int pos_cond;
  int pos;
  char **conditional_saved;
  char **conditional_temp;
  a_type conditional_evald;
  a_type block_evald;

  func temp_scope =
    {
      "while_temp",
      NULL,
      NULL,
      1,
      NULL,
      NULL,
      scope,
      NULL,
      NULL
    };

  extern int get_exp_length_first (char **, int);

  if (words[0] == NULL || words[0][0] != '(')
    return errorman_throw_reg (scope, "expected '(' after while");

  pos_cond = get_exp_length (words + 1, ')');
  conditional_saved = (char **) better_malloc ((sizeof (char *)) * (pos_cond + 1));

  conditional_saved[pos_cond] = NULL;
  pos = pos_cond;

  while (pos > 0)
    {
      pos--;
      conditional_saved[pos] = words[pos + 1];
    }

  if (words[pos_cond] == NULL)
    return errorman_throw_reg (scope, "syntax error in while loop");

  block_evald.type = VAR_TYPE;
  block_evald.v_point = alloc_var ();
  block_evald.break_signal = false;

  for (;;)
    {
      conditional_temp = copy (conditional_saved);

      conditional_evald = eval (&temp_scope, conditional_temp);
      
      if (conditional_evald.type == ERROR_TYPE)
	return conditional_evald;
      
      if (conditional_evald.type == FUNCTION_TYPE)
	return errorman_throw_reg (scope, "while loop conditional must return a var");

      if (mpc_cmp_si (conditional_evald.v_point->data, 0) == 0)
        break;

      block_evald = expression (&temp_scope, words + pos_cond + 1);

      if (block_evald.type == ERROR_TYPE || block_evald.isret == true || block_evald.break_signal == true)
	break;
    }

  return block_evald;
}

a_type
for_loop (func *scope, char **words)
{
  int pos;
  int arr_pos;
  a_type index_value;
  a_type limit_value;
  a_type block_evald;
  var *var_scroller;
  func *func_scroller;
  mpc_t one;

  func temp_scope =
    {
      "for_temp",
      NULL,
      NULL,
      1,
      NULL,
      NULL,
      scope,
      NULL,
      NULL
    };

  index_value = eval (&temp_scope, words);

  if (index_value.type == ERROR_TYPE)
    return index_value;

  if (strcmp (words[1], ","))
    return errorman_throw_reg (scope, "syntax error in for loop; missing ','");

  limit_value = eval (&temp_scope, words + 2);

  if (limit_value.type == ERROR_TYPE)
    return limit_value;

  if (limit_value.type != index_value.type)
    return errorman_throw_reg (scope, "error in for loop; index type does not match destination type");

  if (strcmp (words[3], "then"))
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

	      for (var_scroller = limit_value.v_point->array_up, pos = 0;
		   pos < arr_pos; pos++)
		var_scroller = var_scroller->next;

	      index_value.v_point->array_size = var_scroller->array_size;
	      mpc_set (&(index_value.v_point->data), var_scroller->data);
	      index_value.v_point->array_up = clone_var (var_scroller->array_up, index_value.v_point->name);
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

	      for (func_scroller = limit_value.f_point->array_up, pos = 1;
		   pos < arr_pos; pos++)
		func_scroller = func_scroller->next;

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
	    return errorman_throw_reg (scope, "error in for loop; if the destination variable is a function, it must also be an array");
	}

      if (strcmp (words[4], ";"))
	block_evald = expression (&temp_scope, words + 4);

      if (block_evald.type == ERROR_TYPE || block_evald.isret == true || block_evald.break_signal == true)
	break;

      arr_pos++;
    }

  return block_evald;
}


