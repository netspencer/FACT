#include "common.h"

FACT_t
invalid_if (func_t *scope, word_list expression)
{
  return errorman_throw_reg (scope, "invalid syntax, if statements must start at the beginning of the expression");
}

FACT_t
invalid_on_error (func_t *scope, word_list expression)
{
  return errorman_throw_reg (scope, "invalid syntax, on_error statements must start at the beginning of the expression");
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
      .line       = scope->line,
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
  scope->line = temp_scope.line;

  if (conditional.type == ERROR_TYPE)
    {
      conditional.error.scope = scope;
      return conditional;
    }

  if (conditional.type == FUNCTION_TYPE)
    return errorman_throw_reg (scope, "if statement conditional must return a var");
  
  if (mpc_cmp_si (conditional.v_point->data, 0) == 0)
    {
      return_value.v_point       = alloc_var ();
      return_value.type          = VAR_TYPE;
      return_value.return_signal = false;
      return_value.break_signal  = false;
      *success                   = false;
      scope->line                = temp_scope.line;
      
      return return_value;
    }

  while (expression_list.move_forward[0])
    {
      expression_list.syntax++;
      expression_list.move_forward++;
      expression_list.lines++;
    }
  
  return_value = eval_expression (&temp_scope, expression_list);
  scope->line  = temp_scope.line;
  
  if (return_value.type == ERROR_TYPE)
    return_value.error.scope = scope;
    
  return return_value;
}

FACT_t
on_error (func_t *scope, word_list expression_list, bool *success)
{
  /*
    Since on_error statements act pretty much the same way as
    if statements, this function is basically a facsimile of
    the function if_statement.
  */

  var_t  * message;
  FACT_t   return_value;
  FACT_t   conditional;
  func_t * ERROR;
  func_t   temp_scope =
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
    return errorman_throw_reg (scope, "expected '(' after on_error statement");

  conditional = eval (&temp_scope, expression_list);

  if (conditional.type == ERROR_TYPE && !conditional.error.thrown)
    {
      conditional.error.scope = scope;
      return conditional;
    }
  
  if (conditional.type != ERROR_TYPE)
    {
      return_value.v_point       = alloc_var ();
      return_value.type          = VAR_TYPE;
      return_value.return_signal = false;
      return_value.break_signal  = false;
      *success                   = false;  
      
      return return_value;
    }

  while (expression_list.move_forward[0])
    {
      expression_list.syntax++;
      expression_list.move_forward++;
      expression_list.lines++;
    }
  
  /*
   * This adds a local object called 'ERROR', which only
   * Can be accessed in the on_error's scope.
   *
   * ERROR:
   *  - locked => true
   *  - name   => 'ERROR'
   *  - vars   =>
   *  | - message => the error message of the caught exception.
   *  | | - locked => true
   *  - up     => the scope of the caught exception.
   */
  ERROR               = add_func (&temp_scope, "ERROR");
  ERROR->locked       = true;
  ERROR->up           = conditional.error.scope;
  message             = add_var (ERROR, "message");
  message->array_up   = string_to_array (conditional.error.description, "message");
  message->array_size = strlen (conditional.error.description);
  message->locked     = true;
  
  return_value = eval_expression (&temp_scope, expression_list);

  if (return_value.type == ERROR_TYPE)
    return_value.error.scope = scope;
    
  return return_value;
}

FACT_t
else_clause (func_t *scope, word_list expression)
{
  FACT_t return_value;  
  func_t temp_scope =
    {
      .name       = scope->name,
      .line       = scope->line,
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

  return_value = eval_expression (&temp_scope, expression);
  scope->line  = temp_scope.line; 

  if (return_value.type == ERROR_TYPE)
    return errorman_throw_reg (scope, return_value.error.description);

  return return_value;
}

FACT_t
while_loop (func_t *scope, word_list expression)
{
  //int       pos_cond;
  //int       pos;
  int       jump_len;
  int       index;
  int       exp_len;
  FACT_t    conditional_evald;
  FACT_t    block_evald;
  word_list conditional_exp;
  func_t    temp_scope =
    {
      .line       = scope->line,
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

  /*
  if (words[0] == NULL || words[0][0] != '(')
  */
  if (expression.syntax[0] == NULL || expression.syntax[0][0] != '(')
    return errorman_throw_reg (scope, "expected '(' after while");

  /*
  pos_cond                     = get_exp_length (words + 1, ')');
  conditional_exp.syntax       = words;
  conditional_exp.move_forward = better_malloc (sizeof (bool) * pos_cond);
  conditional_exp.lines        = better_malloc (sizeof (int ) * pos_cond);
  pos                          = pos_cond;
  */

  /*
  if (words[pos_cond] == NULL)
    return errorman_throw_reg (scope, "syntax error in while loop");
  */

  block_evald.type         = VAR_TYPE;
  block_evald.v_point      = alloc_var ();
  block_evald.break_signal = false;
  jump_len                 = 0;
  exp_len                  = 0;

  for (;;)
    {
      expression.syntax       -= jump_len;
      expression.move_forward -= jump_len;
      expression.lines        -= jump_len;
      temp_scope.line          = scope->line;
      jump_len                 = 0;
      
      for (index = 0; expression.syntax[index] != NULL; index++)
	expression.move_forward[index] = false;
      
      conditional_evald = eval (&temp_scope, expression);
      
      if (conditional_evald.type == ERROR_TYPE)
	{
	  scope->line = temp_scope.line;
	  return conditional_evald;
	}
      if (conditional_evald.type == FUNCTION_TYPE)
	{
	  scope->line = temp_scope.line;
	  return errorman_throw_reg (scope, "while loop conditional must return a var_t");
	}

      if (mpc_cmp_si (conditional_evald.v_point->data, 0) == 0)
        break;

      while (expression.move_forward[0])
	{
	  expression.move_forward[0] = false;
	  expression.syntax++;
	  expression.move_forward++;
	  expression.lines++;
	  jump_len++;
	}

      block_evald = eval_expression (&temp_scope, expression);

      if (block_evald.type == ERROR_TYPE || block_evald.return_signal == true || block_evald.break_signal == true)
	break;
      /*
#ifdef DEBUG
      printf ("exp_len : before = %d\n", exp_len);
#endif
      for (exp_len = 0; expression.move_forward[exp_len]; exp_len++);
#ifdef DEBUG
      exp_len += jump_len;
      printf ("exp_len : after  = %d\n", exp_len);
#endif
      */

       //set_array (conditional_exp.move_forward, pos_cond + 1);
    }
  scope->line = temp_scope.line;

  /* Something needs to go here in order to move the expression forward.
     I am not so sure as to if this works or not:
  */
  /*
#ifdef DEBUG
  printf ("exp_len = %d\n", exp_len);
#endif
  for (index = 0; index < exp_len; index++)
    expression.move_forward[index] = true;
  */
  return block_evald;
}

FACT_t
for_loop (func_t *scope, word_list expression)//char **words)
{
  /*
    I would say that this function could be cleaned up
    a fairly large amount.
  */
  int         index;
  int         pos;
  int         count;
  int         arr_pos;
  int         hold_lines;
  FACT_t      index_value;
  FACT_t      limit_value;
  FACT_t      block_evald;
  //  word_list   index_dest_exp;
  mpc_t       one;
  var_t     * var_t_scroller;
  func_t    * func_t_scroller;
  func_t      temp_scope =
    {
      .line       = scope->line,
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

  //index_dest_exp.syntax       = words;
  //index_dest_exp.move_forward = better_malloc (sizeof (int) *
  //					       ((count = count_until_NULL (words)) + 1));
  index_value = eval (&temp_scope, expression);

  if (index_value.type == ERROR_TYPE)
    {
      scope->line = temp_scope.line;
      return index_value;
    }

  while (expression.move_forward[0])
    {
      expression.syntax++;
      expression.move_forward++;
      expression.lines++;
    }

  if (tokcmp (expression.syntax[0], ","))
    {
      scope->line = temp_scope.line;
      return errorman_throw_reg (scope, "syntax error in for loop; missing ','");
    }

  temp_scope.line           += expression.lines[0];
  expression.move_forward[0] = true;
  limit_value                = eval (&temp_scope, expression);

  if (limit_value.type == ERROR_TYPE)
    {
      scope->line = temp_scope.line;
      return limit_value;
    }

  if (limit_value.type != index_value.type)
    {
      scope->line = temp_scope.line;
      return errorman_throw_reg (scope, "error in for loop; index type does not match destination type");
    }

  while (expression.move_forward[0])
    {
      expression.syntax++;
      expression.move_forward++;
      expression.lines++;
    }
  
  if (tokcmp (expression.syntax[0], "then"))
    {
      scope->line = temp_scope.line;
      return errorman_throw_reg (scope, "syntax error in for loop; missing 'then'");
    }

  temp_scope.line += expression.lines[0];

  expression.syntax++;
  expression.move_forward++;
  expression.lines++;

  mpc_init (&one);
  mpc_set_ui (&one, 1);

  arr_pos                  = 0;
  block_evald.type         = VAR_TYPE;
  block_evald.v_point      = alloc_var ();
  block_evald.break_signal = false;
  hold_lines               = temp_scope.line;

  for (;;)
    {
      temp_scope.line = hold_lines;
      if (index_value.type == VAR_TYPE)
	{
	  if (limit_value.v_point->array_size > 1)
	    {
	      if (arr_pos >= limit_value.v_point->array_size)
		break;

	      for (var_t_scroller = limit_value.v_point->array_up, pos = 0;
		   pos < arr_pos; pos++)
		var_t_scroller = var_t_scroller->next;

	      mpc_set (&(index_value.v_point->data), var_t_scroller->data);
	      index_value.v_point->array_size = var_t_scroller->array_size;
	      index_value.v_point->array_up   = clone_var (var_t_scroller->array_up, index_value.v_point->name);
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

	      index_value.f_point->args       = limit_value.f_point->args;
	      index_value.f_point->body       = limit_value.f_point->body;
	      index_value.f_point->array_size = limit_value.f_point->array_size;
	      index_value.f_point->vars       = limit_value.f_point->vars;
	      index_value.f_point->funcs      = limit_value.f_point->funcs;
	      index_value.f_point->up         = limit_value.f_point->up;
	      index_value.f_point->array_up   = limit_value.f_point->array_up;
	      index_value.f_point->next       = limit_value.f_point->next;
	    }
	  else
	    {
	      scope->line = temp_scope.line;
	      return errorman_throw_reg (scope, "error in for loop; if the destination variable is a function, it must also be an array");
	    }
	}

      if (strcmp (expression.syntax[0], ";"))
	block_evald = eval_expression (&temp_scope, expression);

      if (block_evald.type == ERROR_TYPE || block_evald.return_signal == true || block_evald.break_signal == true)
	break;

      for (index = 0; expression.syntax[index] != NULL; index++)
	expression.move_forward[index] = false;

      arr_pos++;
    }
  scope->line = temp_scope.line;

  return block_evald;
}


