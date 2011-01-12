#include "common.h"

#define OPEN_SUCCESS 2
#define OPEN_FAILED  1
#define CLOSED       0

/* FACT Bytcode simple guide:
 * Scripts are translated into bytecode once after execution. Bytecode compiled code
 * works within the same word_list structure as non-compiled code. The first byte
 * in an instruction begins with a 1, to indicate that it is in fact an instruction.
 * The next byte indicates the category of the instruction. This can either be
 * STATEMENT (0x1), MATH_CALL (0x2), PRIMITIVE (0x3), NUMBER (0x4), VARIABLE (0x5),
 * or FUNCTION (0x6). The next byte's format is dependent on the code's category.
 * Instruction 0x10 represents the end of an expression, which is used by 
 * get_exp_length and get_exp_length_first to speed things up.
 *
 * TODO: Add more things.
 */

typedef enum bytecode_type 
  {
    STATEMENT = 0x1, /* This includes keywords such as if, else, and return. */
    MATH_CALL = 0x2, /* +, -, *, etc. */
    PRIMITIVE = 0x3, /* Some special things. */
    NUMBER    = 0x4, /* Numbers become stored in a special way, will implement later. */
    VARIABLE  = 0x5, /* Address of a variable. */
    FUNCTION  = 0x6, /* Address of a function. */
  } bytecode_type;

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
  int    index;
  int    jndex;
  bool   write_bcode;   /* false = current token is bytecode, true = write bytecode. */
  bool   return_signal;
  bool   break_signal;
  bool   getif;
  FACT_t return_value;
  static int            depth          = -1;
  static int          * if_open        = NULL;
  static unsigned int   size_allocated = 0;

  /* This function checks the begining of the expression for
   * statements such as if, while, or etc, and for signals
   * such as break or return. Since this function is called
   * with every iteration in a recursive function, so it
   * handles recursion depth as well.
   */

#define BREAK_SIG 0 /* This number is arbitrary, and as long as it's different from
		       IF, ON_ERROR, ELSE, WHILE, FOR, and RETURN_STAT then it will
		       work. */
#define STAT_SIZE (((sizeof (statements)) / (sizeof (statements[0]))) - 1)

  /* The reason why I'm using a struct instead of just an array of
   * char pointers is simply because this makes the interface a fair
   * amount cleaner.
   */
  static struct
  {
    int    id;
    char * token;
  } statements [] =
      {
	{ IF          , "if"       },
	{ ON_ERROR    , "on_error" },
	{ ELSE        , "else"     },
	{ WHILE       , "while"    },
	{ FOR         , "for"      },
	{ OP_CURLY    , "{"        },
	{ RETURN_STAT , "return"   },
	{ BREAK_SIG   , "break"    },
 	{ UNKNOWN     , NULL       }, /* This ends the struct, again used for interface reasons. */
      };

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
      && expression.syntax[0][1] == 0x1)
    {
      write_bcode = false;
      index       = (int) expression.syntax[0][2];
    }
  else
    {
      /* ---- Otherwise, search for a statement ---- */
      write_bcode = true;
      for (index = 0; index < STAT_SIZE; index++)
	{
	  if (!tokcmp (statements[index].token, expression.syntax[0]))
	    break;
	}
    }
  /* ---- Do some things if it is a statement ---- */
  if (index < STAT_SIZE)
    {
      /* If it's desired, write the bytecode. */
      if (write_bcode)
	{
	  //better_free (expression.syntax[0]);
	  expression.syntax[0]    = better_malloc (sizeof (char) * 3);
	  expression.syntax[0][0] = 0x1;
	  expression.syntax[0][1] = 0x1;
	  expression.syntax[0][2] = (char) index;
	}
      scope->line += expression.lines[0];
      expression.move_forward[0] = true;
      expression.syntax++;
      expression.move_forward++;
      expression.lines++;
    }
  /* ---- If it's not an if/on_error/else statement, ----
     ---- close all opened conditional blocks.       ---- */
  if (statements[index].id != IF
      && statements[index].id != ON_ERROR
      && statements[index].id != ELSE)
    {
      for (jndex = size_allocated - 1; jndex >= depth; jndex--)
	if_open[jndex] = CLOSED;
    }
  /* ---- If it's an if or an on_error statement ---- */
  if (statements[index].id == IF || statements[index].id == ON_ERROR)
    {
      /* ---- Check if it's an on_error or if statement ---- */
      if (statements[index].id == IF)
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
	      /*
		if (temp_depth >= MAX_RECURSION)
		return errorman_throw_reg (scope, "You have too many nested if loops. They exceed the maximum recursion depth");
	      */

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
  else if (statements[index].id == ELSE)
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
		  /*
		  if (temp_depth >= MAX_RECURSION)
		    return errorman_throw_reg (scope, "You have too many nested if loops. They exceed the maximum recursion depth");
		  */

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
  else if (statements[index].id == WHILE)
    return_value = while_loop (scope, expression);
  else if (statements[index].id == FOR)
    return_value = for_loop (scope, expression);
  else if (statements[index].id == OP_CURLY)
    {
      return_value = lambda_proc (scope, expression);
      for (index = depth + 1; index < size_allocated; index++)
	if_open[index] = CLOSED;
    }
  else if (statements[index].id == RETURN_STAT)
    {
      return_value  = eval (scope, expression);
      return_signal = true;
    }
  else if (statements[index].id == BREAK_SIG)
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
  int      bytes;
  int      call_num;
  char   * current_token;
  var_t  * hold_var;
  FACT_t   return_value;
  unsigned long hold_pointer;

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
  /* Check to see if the current token is valid bytecode, 
   * and if so, call the desired instruction.
   */
  if (current_token[0] == 0x1)
    {
      /* ---- Move the expression forward one ---- */
      expression.syntax++;
      expression.move_forward++;
      expression.lines++;
      /* check which category it's from */
      if (current_token[1] == MATH_CALL)
	return_value = eval_math (scope, expression, (int) current_token[2]);
      else if (current_token[1] == PRIMITIVE)
	return_value = runprim (scope, expression, (int) current_token[2]);
      else if (current_token[1] == VARIABLE)
	{
	  return_value.type = VAR_TYPE;
	  hold_pointer = 0;
	  bytes = (sizeof (var_t *) / sizeof (char));
	  for (index = 0; index < bytes; index++)
	    {
#if (BYTE_ORDER == BIG_ENDIAN)
	      hold_pointer >>= 8;
#else /* We assume big and little are the only values available */
	      hold_pointer <<= 8;
#endif
	      hold_pointer += (unsigned char) current_token[index + 2];
	    }
	  return_value.v_point = (var_t *) hold_pointer;
	}
      
      if (return_value.type == VAR_TYPE)
	return_value = get_array_var (return_value.v_point, scope, expression);
    }
  /* ---- Check if the current token is a number ---- */
  else if (isnum (current_token))
    return_value = num_to_var (current_token);
  else
    {
      /* ---- Move the expression forward one ---- */
      expression.syntax++;
      expression.move_forward++;
      expression.lines++;
      /* ---- Check for primitives, math calls, variables and functions, in that order ---- */
      if ((call_num = isprim (current_token)) != -1)
	{
	  return_value = runprim (scope, expression, call_num);
	  /* This is actually useful. */
	  if (return_value.type == VAR_TYPE)
	    return_value = get_array_var (return_value.v_point, scope, expression);
	  /* This is being removed for a little bit while
	     I plan some stuff out...
	  expression.syntax[-1]    = better_malloc (sizeof (char) * 3);
	  expression.syntax[-1][0] = 0x1;
	  expression.syntax[-1][1] = PRIMITIVE;
	  expression.syntax[-1][2] = (char) call_num;
	  */
	}
      else if ((call_num = ismathcall (current_token)) != -1)
	{
	  return_value = eval_math (scope, expression, call_num);
	  /* I don't see how this could ever be useful, but whatever. */
	  if (return_value.type == VAR_TYPE)
	    return_value = get_array_var (return_value.v_point, scope, expression);
	  expression.syntax[-1]    = better_malloc (sizeof (char) * 3);
	  expression.syntax[-1][0] = 0x1;
	  expression.syntax[-1][1] = MATH_CALL;
	  expression.syntax[-1][2] = (char) call_num;
	}
      else if ((hold_var = get_var (scope, current_token)) != NULL)
	{
	  return_value = get_array_var (hold_var, scope, expression);
	  bytes = (sizeof (var_t *) / sizeof (char));
	  expression.syntax[-1] = better_malloc (sizeof (char) * (2 + bytes));
	  expression.syntax[-1][0] = 0x1;
	  expression.syntax[-1][1] = VARIABLE;
	  hold_pointer = (unsigned long) hold_var;
	  for (index = 1; index <= bytes; index++)
	    {
#if (BYTE_ORDER == BIG_ENDIAN)
	      expression.syntax[-1][index + 1] = (char) (hold_pointer << (bytes - index) * 8);
#else
	      expression.syntax[-1][index + 1] = (char) (hold_pointer >> ((bytes - index) * 8));
#endif
	    }
	}     
      else if (get_func (scope, current_token) != NULL)
	return_value = get_array_func (get_func (scope, current_token), scope, expression);
      else
	return errorman_throw_reg (scope, combine_strs ("cannot evaluate ", current_token));
    }
  /* ---- Set the signals to off ---- */
  return_value.return_signal = false;
  return_value.break_signal  = false;

  return return_value;
}
