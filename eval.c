#include "common.h"

#define OPEN_SUCCESS 2
#define OPEN_FAILED  1
#define CLOSED       0

word_list
make_word_list (char **words, bool len_check)
{
  int       index;
  int       jndex;
  word_list expression;

  if (len_check)
    {
      /* ---- Find the length of the statement ---- */
      index = get_exp_length_first (words, ';');
      /* ---- Check if there were any end-statement syntax errors ----
	 This will be removed in the near future as soon as I add
	 (and fix ;) the parser's error checker. */
      if (index < 1)
	//	  || (tokcmp (words[index - 1], ";")
	//    && tokcmp (words[index - 1], "}")))
	{
	  expression.syntax = NULL;
	  return expression;
	}
    }
  else
    {
      /* ---- Get the length until the NULL terminator ---- */
      for (index = 0; words[index] != NULL; index++)
	;
    }
  /* ---- Allocate the memory for the expression ---- */
  expression.syntax       = better_malloc (sizeof (char *) * (index + 1));
  expression.move_forward = better_malloc (sizeof (bool  ) * (index + 1));
  expression.lines        = better_malloc (sizeof (int   ) * (index + 1));
  /* ---- Fill the memory ---- */
  for (expression.syntax[index--] = NULL; index >= 0; index--)
    {
      for (jndex = 0; words[index][jndex] == '\n'; jndex++);
      if (words[index] + jndex == '\0')
	expression.syntax[index] = NULL;
      else
	expression.syntax[index] = words[index] + jndex;
      expression.lines[index] = jndex;
    }
  
  return expression;
}

FACT_t
eval_expression (func_t *scope, word_list expression)
{
  
  /* This function checks the begining of the expression for
   * statements such as if, while, or etc, and for signals
   * such as break or return. Since this function is called
   * with every iteration in a recursive function, so it
   * handles recursion depth as well.
   */
  int    index;
  int    jndex;
  byte   instruction;
  bool   is_instruction;
  bool   return_signal;
  bool   break_signal;
  bool   getif;
  FACT_t return_value;
  static int            depth          = -1;
  static int          * if_open        = NULL;
  static unsigned int   size_allocated = 0;

  /* ---- Set the default return_value type ----
     This probably isn't necessary. */
  return_value.type = VAR_TYPE;
  /* ---- Set the signals to their defaults ---- */
  return_signal = false;
  break_signal  = false;
  /* ---- Increase the recursion depth ---- */
  depth++;
  /* ---- Allocate the recursion stack ---- */
  if (depth >= size_allocated)
    {
      size_allocated  = depth + 1;
      if_open         = better_realloc (if_open, sizeof (int) * size_allocated);
    }
  /* ---- Check for an invalid statement ----
     Note: This will be removed in the future
     when I fix the parser's error checker. */
  if (expression.syntax == NULL) 
    {
      depth--;
      return errorman_throw_reg (scope, "syntax error: expected closing ';' or '}'");
    }
  /* Check to see if it's a bytecode instruction of the
   * STATEMENT category. */
  if (expression.syntax[0][0] == 0x1
      && expression.syntax[0][1] == 0x2)
    {
      is_instruction = true;
      instruction    = expression.syntax[0][2];
      /* If it's not an if/on_error/else statement, close all
       * open conditional blocks.
       */
      if (instruction != IFS 
	  && instruction != ONE 
	  && instruction != ELS)
	{
	  for (jndex = size_allocated - 1; jndex >= depth; jndex--)
	    if_open[jndex] = CLOSED;
	}
      /* Move the stuff forward. */
      scope->line += expression.lines[0];
      expression.move_forward[0] = true;
      expression.syntax++;
      expression.move_forward++;
      expression.lines++;
    }
  else
    is_instruction = false;
  /* ---- If it's an if or an on_error statement ---- */
  if (is_instruction && (instruction == IFS || instruction == ONE))
    {
      /* ---- Check if it's an on_error or if statement ---- */
      if (instruction == IFS)
	return_value = if_statement (scope, expression, &getif);
      else
	return_value = on_error (scope, expression, &getif);

      if (getif)
	if_open[depth] = OPEN_SUCCESS;
      else
	{
	  if_open[depth] = OPEN_FAILED;
	  for (index = 1, jndex = depth + 1; expression.syntax[index] != NULL; index++)
	    {
	      scope->line   += expression.lines[index];

	      if (jndex >= size_allocated)
		{
		  size_allocated = jndex + 1;
		  if_open        = better_realloc (if_open, sizeof (int) * size_allocated);
		}
	      if (!tokcmp (expression.syntax[index], "\""))
		{
		  if (expression.syntax[index + 2] != NULL)
		    scope->line += expression.lines[index + 2];
		  scope->line += expression.lines[index + 1];
		  index       += 2;
		}

	      if (!strcmp (expression.syntax[index], "if")
		  || !strcmp (expression.syntax[index], "on_error"))
		if_open[jndex++] = OPEN_SUCCESS;
	      else if (!strcmp (expression.syntax[index], ";")
		       || !strcmp (expression.syntax[index], "{"))
		break;
	    }
	}
    }
  else if (is_instruction && instruction == ELS)
    {
      for (index = size_allocated - 1; index >= 0; index--)
	{
	  if (if_open[index] == OPEN_FAILED)
	    {
	      jndex        = depth;
	      depth        = index - 1;
	      return_value = else_clause (scope, expression);
	      depth        = jndex;
	      break;
	    }
	  else if (if_open[index] == OPEN_SUCCESS)
	    {
	      if_open[index] = CLOSED;
	      jndex          = index;
	      for (index = 0; expression.syntax[index] != NULL; index++)
		{
		  scope->line += expression.lines[index];

		  if (jndex >= size_allocated)
		  {
		    size_allocated = jndex + 1;
		    if_open        = better_realloc (if_open, sizeof (int) * size_allocated);
		  }
		  if (!tokcmp (expression.syntax[index], "\""))
		    {
		      if (expression.syntax[index + 2] != NULL)
			scope->line += expression.lines[index + 2];
		      scope->line += expression.lines[index + 1];
		      index       += 2;
		    }
		  if (!tokcmp (expression.syntax[index], "if")
		      || !tokcmp (expression.syntax[index], "on_error"))
		    if_open[jndex++] = OPEN_SUCCESS;
		  else if (!tokcmp (expression.syntax[index], ";")
			   || !tokcmp (expression.syntax[index], "{"))
		    break;
		}
	      depth--;
	      return FACT_get_ui (0);
	    }
	}
      if (index == -1)
	{
	  depth--;
	  return errorman_throw_reg (scope, "unmatched 'else'");
	}
    }
  else if (is_instruction && instruction == WHL)
    return_value = while_loop (scope, expression);
  else if (is_instruction && instruction == FRL)
    return_value = for_loop (scope, expression);
  else if (is_instruction && instruction == CRL)
    {
      return_value = lambda_proc (scope, expression);
      for (index = depth + 1; index < size_allocated; index++)
	if_open[index] = CLOSED;
    }
  else if (is_instruction && instruction == RTN)
    {
      return_value  = eval (scope, expression);
      return_signal = true;
    }
  else if (is_instruction && instruction == BRK)
    {
      if (expression.syntax[0] != NULL
	  && tokcmp (expression.syntax[0], ";"))
	return errorman_throw_reg (scope, "break must be alone in an expression");
      break_signal = true;
    }
  else
    return_value = eval (scope, expression);
  /* ---- Get to the last end of the expression ---- */
  while (expression.move_forward[0])
    {
      expression.syntax++;
      expression.move_forward++;
      expression.lines++;
    }
  /* ---- Check if ';' terminates the expression ---- */
  if (expression.syntax[0] != NULL
      && !tokcmp (expression.syntax[0], ";"))
    expression.move_forward[0] = true;
  /* ---- Set the signals to their correct values ---- */
  if (!return_value.return_signal)
    return_value.return_signal = return_signal;
  if (!return_value.break_signal)
    return_value.break_signal = break_signal;
  /* ---- Get out of this depth and exit ---- */
  depth--;
  return return_value;
}

FACT_t
procedure (func_t *scope, word_list expression)
{
  int    length_to_move;
  int    index;
  FACT_t return_value; 

  while (expression.syntax[0] != NULL
	 && tokcmp (expression.syntax[0], "}"))
    {
      length_to_move = get_exp_length_first (expression.syntax, ';');
      return_value   = eval_expression (scope, expression);

      if (return_value.type == ERROR_TYPE
	  || return_value.return_signal
	  || return_value.break_signal)
	{
	  /* I'm not using length_to_move as the index
	   * variable as that is slightly misleading.
	   * Also this sets all the remaining tokens to
	   * evaluated = true.
	   */
	  for (index = 0; index < get_exp_length (expression.syntax, '}');
	       index++)
	    expression.move_forward[index] = true;
	  return return_value;
	}

      expression.move_forward += length_to_move;
      expression.syntax       += length_to_move;
      expression.lines        += length_to_move;
    }

  expression.move_forward[0] = true;
  return_value.return_signal = false;
  return_value.break_signal  = false;
  return_value.v_point       = alloc_var ();
  return_value.type          = VAR_TYPE;
      
  return return_value;
}

FACT_t
lambda_proc (func_t *scope, word_list expression)
{
  FACT_t return_value;
  func_t temp_local = 
    {
      .line       = scope->line      ,
      .name       = scope->name      ,  
      .file_name  = scope->file_name ,
      .args       = NULL             ,
      .body       = NULL             ,
      .usr_data   = NULL             ,
      .extrn_func = NULL             ,
      .vars       = NULL             ,
      .funcs      = NULL             ,
      .up         = scope            ,
      .caller     = NULL             ,
      .array_up   = NULL             ,
      .next       = NULL             ,
      .variadic   = NULL             ,
    };

   /* temp_local is a temporary scope created to contain
    * lambda procedures. Note, these are different from
    * lambda functions. A lot.
    */

  mpz_init_set_ui (temp_local.array_size, 1);
  
  return_value = procedure (&temp_local, expression);
  scope->line  = temp_local.line;

  return return_value;
}

FACT_t
eval (func_t *scope, word_list expression)
{
  int      index;
  int      call_num;
  char   * current_token;
  var_t  * hold_var;
  func_t * hold_func;
  FACT_t   return_value;
  unsigned long hold_pointer;
  unsigned byte bytes;

  /* ---- Move the expression forward to the next unevaluated token ---- */
  while (expression.move_forward[0])
    {
      expression.syntax++;
      expression.move_forward++;
      expression.lines++;
    }
  /* ---- Set the current token ---- */
  current_token              = expression.syntax[0];
  /* ---- Set the current token to evaluated and move the current line number forward ---- */
  expression.move_forward[0] = true;
  scope->line               += (expression.lines != NULL) ? expression.lines[0] : 0;
  /* ---- Move the expression forward one ---- */
  expression.syntax++;
  expression.move_forward++;
  expression.lines++; 
  /* Check to see if the current token is valid bytecode, 
   * and if so, call the desired instruction.
   */
  if (current_token[0] == BYTECODE)
    {
      /* check which category it's from */
      if (current_token[1] == MATH_CALL)
	return_value = eval_math (scope, expression, (int) current_token[2]);
      else if (current_token[1] == NUMBER)
	{
	  return_value.type = VAR_TYPE;
	  hold_pointer = 0;
	  bytes = (sizeof (var_t *) / sizeof (char));
	  for (index = 0; index < bytes; index++)
	    {
#if (BYTE_ORDER == LITTLE_ENDIAN)
	      hold_pointer <<= 8;
#else /* We assume big and little are the only values available */
	      hold_pointer >>= 8;
#endif
	      hold_pointer += (unsigned char) current_token[index + 2];
	    }
	  return_value.v_point = (var_t *) hold_pointer;
	}      
    }
  else
    {
      if ((call_num = isprim (current_token)) != -1)
	return_value = runprim (scope, expression, call_num);
      else if ((hold_var = get_var (scope, current_token)) != NULL)
	{
	  return_value.v_point = hold_var;
	  return_value.type    = VAR_TYPE;
	}     
      else if ((hold_func = get_func (scope, current_token)) != NULL)
	{
	  return_value.f_point = hold_func;
	  return_value.type    = FUNCTION_TYPE;
	}
      else
	return errorman_throw_reg (scope, combine_strs ("cannot evaluate ", current_token));
    }

  if (return_value.type == VAR_TYPE)
    return_value = get_array_var (return_value.v_point, scope, expression);
  else if (return_value.type == FUNCTION_TYPE)
    return_value = get_array_func (return_value.f_point, scope, expression);

  /* ---- Set the signals to off ---- */
  return_value.return_signal = false;
  return_value.break_signal  = false;
  
  return return_value;
}
