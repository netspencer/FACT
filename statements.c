#include "statements.h"

FACT_t
if_statement (func_t *scope, word_list expression_list, bool *success)
{
  unsigned long ip;
  FACT_t return_value;
  FACT_t conditional;
  func_t temp_scope =
    {
      .line       = scope->line ,
      .name       = scope->name ,
      .args       = NULL        ,
      .body       = NULL        ,
      .usr_data   = NULL        ,
      .extrn_func = NULL        ,
      .vars       = NULL        ,
      .funcs      = NULL        ,
      .up         = scope       ,
      .caller     = NULL        ,
      .array_up   = NULL        ,
      .next       = NULL        ,
      .variadic   = NULL        ,
    };
  
  *success = true;
  mpz_init_set_ui (temp_scope.array_size, 1);

  expression_list.syntax++;
  expression_list.lines++;
  
  if (tokcmp (expression_list.syntax[0], "("))
    return errorman_throw_reg (scope, "expected '(' after if statement");

  ip = get_ip ();
  set_ip (0);
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

  return_value = eval_expression (&temp_scope, expression_list);
  scope->line  = temp_scope.line;
  
  if (return_value.type == ERROR_TYPE)
    return_value.error.scope = scope;
  ip += get_ip ();
    
  return return_value;
}

FACT_t
on_error (func_t *scope, word_list expression_list, bool *success)
{
  /* Since on_error statements act pretty much the same way as
   * if statements, this function is basically a facsimile of
   * the function if_statement.
   */

  var_t  * message;
  FACT_t   return_value;
  FACT_t   conditional;
  func_t * ERROR;
  func_t   temp_scope =
    {
      .line       = scope->line ,
      .name       = scope->name ,
      .args       = NULL        ,
      .body       = NULL        ,
      .usr_data   = NULL        ,
      .extrn_func = NULL        ,
      .vars       = NULL        ,
      .funcs      = NULL        ,
      .up         = scope       ,
      .caller     = NULL        ,
      .array_up   = NULL        ,
      .next       = NULL        ,
      .variadic   = NULL        ,
    };
  
  *success = true;

  mpz_init_set_ui (temp_scope.array_size, 1);
  
  if (tokcmp (expression_list.syntax[0], "(")
      && (expression_list.syntax[0][0] != 0x1
	  || expression_list.syntax[0][1] != 0x3
	  || expression_list.syntax[0][2] != 0x5))
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
   *  - name => 'ERROR'
   *  - vars =>
   *  | - message => the error message of the caught exception.
   *  | | - locked => true
   *  - up => the scope of the caught exception.
   */
  
  ERROR               = add_func (&temp_scope, "ERROR");
  ERROR->locked       = true;
  ERROR->up           = conditional.error.scope;
  message             = add_var (ERROR, "message");
  message->array_up   = string_to_array (conditional.error.description, "message");
  message->locked     = true;
  mpz_set_si (message->array_size, strlen (conditional.error.description));
  
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
      .line       = scope->line ,
      .name       = scope->name ,
      .args       = NULL        ,
      .body       = NULL        ,
      .usr_data   = NULL        ,
      .extrn_func = NULL        ,
      .vars       = NULL        ,
      .funcs      = NULL        ,
      .up         = scope       ,
      .caller     = NULL        ,
      .array_up   = NULL        ,
      .next       = NULL        ,
      .variadic   = NULL        ,
    };
  
  expression.syntax++;
  expression.lines++;

  mpz_init_set_ui (temp_scope.array_size, 1);

  reset_ip ();
  return_value = eval_expression (&temp_scope, expression);
  scope->line  = temp_scope.line; 

  if (return_value.type == ERROR_TYPE)
    return errorman_throw_reg (scope, return_value.error.description);

  return return_value;
}

FACT_t
while_loop (func_t *scope, word_list expression)
{
  int       index;
  FACT_t    conditional_evald;
  FACT_t    block_evald;
  word_list conditional_exp;
  func_t    temp_scope =
    {
      .line       = scope->line ,
      .name       = scope->name ,
      .args       = NULL        ,
      .body       = NULL        ,
      .usr_data   = NULL        ,
      .extrn_func = NULL        ,
      .vars       = NULL        ,
      .funcs      = NULL        ,
      .up         = scope       ,
      .caller     = NULL        ,
      .array_up   = NULL        ,
      .next       = NULL        ,
      .variadic   = NULL        ,
    };

  mpz_init_set_ui (temp_scope.array_size, 1);

  expression.syntax++;
  expression.lines++;

  if (expression.syntax[0] == NULL || expression.syntax[0][0] != '(')
    return errorman_throw_reg (scope, "expected '(' after while");

  block_evald.type         = VAR_TYPE;
  block_evald.v_point      = alloc_var ();
  block_evald.break_signal = false;

  for (;;)
    {
      temp_scope.line = scope->line;

      reset_ip ();
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
      
      block_evald = eval_expression (&temp_scope, expression);

      if (block_evald.type == ERROR_TYPE || block_evald.return_signal == true || block_evald.break_signal == true)
	break;
    }
  
  scope->line = temp_scope.line;
  return block_evald;
}

FACT_t
for_loop (func_t *scope, word_list expression)
{
 
  int             index         ;
  int             hold_lines    ;
  mpc_t           one           ;
  var_t         * var_scroller  ;
  FACT_t          index_value   ;
  FACT_t          limit_value   ;
  FACT_t          block_evald   ;
  func_t        * func_scroller ;
  unsigned long   arr_pos       ;
  func_t          temp_scope =
    {
      .line       = scope->line ,
      .name       = scope->name ,
      .args       = NULL        ,
      .body       = NULL        ,
      .usr_data   = NULL        ,
      .extrn_func = NULL        ,
      .vars       = NULL        ,
      .funcs      = NULL        ,
      .up         = scope       ,
      .caller     = NULL        ,
      .array_up   = NULL        ,
      .next       = NULL        ,
      .variadic   = NULL        ,
    };

  mpz_init_set_ui (temp_scope.array_size, 1);

  expression.syntax++;
  expression.lines++;

  reset_ip ();
  index_value = eval (&temp_scope, expression);

  if (index_value.type == ERROR_TYPE)
    {
      scope->line = temp_scope.line;
      return index_value;
    }

  /* Plus one to skip the ',' */
  expression.syntax += get_ip () + 1;
  expression.lines  += get_ip () + 1;

  reset_ip ();
  limit_value = eval (&temp_scope, expression);

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

  /* Since the parser would have rejected the code if a then didn't
   * follow the for statement, we can safely skip the token without
   * worrying about it.
   */
  expression.syntax += get_ip () + 1;
  expression.lines  += get_ip () + 1;

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
	  if (mpz_cmp_ui (limit_value.v_point->array_size, 1) > 0)
	    {
	      if (mpz_cmp_ui (limit_value.v_point->array_size, arr_pos) <= 0)
		break;

	      for (var_scroller = limit_value.v_point->array_up, index = 0;
		   index < arr_pos; index++)
		var_scroller = var_scroller->next;

	      mpc_set (&(index_value.v_point->data), var_scroller->data);
	      mpz_set (index_value.v_point->array_size, var_scroller->array_size);
	      index_value.v_point->array_up   = clone_var (var_scroller->array_up, index_value.v_point->name);
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
	  if (mpz_cmp_ui (limit_value.f_point->array_size, 1) > 0)
	    {
	      if (mpz_cmp_ui (limit_value.v_point->array_size, arr_pos) <= 0)
		break;

	      for (func_scroller = limit_value.f_point->array_up, index = 1;
		   index < arr_pos; index++)
		func_scroller = func_scroller->next;

	      index_value.f_point->args     = limit_value.f_point->args;
	      index_value.f_point->body     = limit_value.f_point->body;
	      index_value.f_point->vars     = limit_value.f_point->vars;
	      index_value.f_point->funcs    = limit_value.f_point->funcs;
	      index_value.f_point->up       = limit_value.f_point->up;
	      index_value.f_point->array_up = limit_value.f_point->array_up;
	      index_value.f_point->next     = limit_value.f_point->next;

	      mpz_set (index_value.f_point->array_size, limit_value.f_point->array_size);
	    }
	  else
	    {
	      scope->line = temp_scope.line;
	      return errorman_throw_reg (scope, "error in for loop; if the destination variable is a function, it must also be an array");
	    }
	}

      reset_ip ();
      if (strcmp (expression.syntax[0], ";"))
	block_evald = eval_expression (&temp_scope, expression);

      if (block_evald.type == ERROR_TYPE || block_evald.return_signal == true || block_evald.break_signal == true)
	break;
    }
  
  scope->line = temp_scope.line;
  return block_evald;
}


