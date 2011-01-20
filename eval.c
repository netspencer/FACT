#include "eval.h"

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
      index = get_exp_length_first (words, ';');
      if (index < 1)
	{
	  expression.syntax = NULL;
	  return expression;
	}
    }
  else
    {
      for (index = 0; words[index] != NULL; index++)
	;
    }

  expression.syntax       = better_malloc (sizeof (char *) * (index + 1));
  expression.move_forward = better_malloc (sizeof (bool  ) * (index + 1));
  expression.lines        = better_malloc (sizeof (int   ) * (index + 1));

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

/* These three variables are used for the handeling of recursion
 * depth, if/on_error blocks, etc. They should generally be left
 * alone.
 */
static int            depth = -1     ;
static int          * if_open        ;       
static unsigned int   size_allocated ;

static void
close_blocks ( void )
{
  /* close_blocks - close all open if/on_error blocks deeper
   * than the current depth. This function is used by
   * functions like lambda_proc to close all inset if/on_error
   * statements.
   */
  unsigned int index;
  
  for (index = depth + 1; index < size_allocated; index++)
    if_open[index] = CLOSED;
}

static void
increase_depth ( void )
{
  /* increase_depth - increase the recursion depth by one,
   * and if appropriate, increase the memory allocated to
   * the recursion stack.
   */

  depth++; // No need to explain this.

  /* If the depth is greater than the size currently
   * allocated, increase size_allocated and allocate
   * more memory.
   */
  if (depth >= size_allocated)
    {
      size_allocated  = depth + 1;
      if_open         = better_realloc (if_open, sizeof (int) * size_allocated);
    }
}

/* This is the instruction pointer, and it points to the next
 * unevaluated token. In order to avoid name space pollution,
 * the variable is accessed through get and set functions, 
 * which act very much like object methods.
 */
static unsigned long ip;

/* Begin instruction pointer accessor methods: */

inline unsigned long
get_ip ( void ) { return ip; }

inline void
set_ip (unsigned long nip) { ip = nip; }

inline void
move_ip (unsigned long nip) { ip += nip; }

inline void
next_inst ( void ) { ip++; }

/* End instruction pointer accessor methods. */

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

  /* Init routines -- Set the default return value to var,
   * set the signals to their default off, and increase
   * the recursion depth by one. 
   */
  return_value.type = VAR_TYPE;  
  return_signal = false;
  break_signal  = false;
  increase_depth ();

  /* Move the current token to the instruction pointer. */
  expression.syntax += get_ip ();
  expression.lines  += get_ip ();

  /* This will be removed in the future. */
  if (expression.syntax == NULL) 
    {
      depth--;
      return errorman_throw_reg (scope, "syntax error: expected closing ';' or '}'");
    }
  
  /* Check to see if it's a bytecode instruction of the
   * STATEMENT category. These are the only instructions
   * that eval_expression checks for.
   */
  if (expression.syntax[0][0] == 0x1
      && expression.syntax[0][1] == 0x2)
    {
      is_instruction = true;
      instruction    = expression.syntax[0][2];

      /* If it's not a conditional statement, close all open blocks. */
      if (instruction != IFS && instruction != ONE && instruction != ELS)
	close_blocks ();

      /* Increse the current line number and instruction pointer
       * by one. This is so statements don't re-evaluated their
       * identifying tokens.
       */
      scope->line += expression.lines[0];
      set_ip (get_ip () + 1);
    }
  else
    is_instruction = false;

  /* The if and on_error statements are combined because
   * they are so similar. After the call their respective
   * functions, they do the exact same thing to the
   * if_open array.
   */
  if (is_instruction && (instruction == IFS || instruction == ONE))
    {
      
      if (instruction == IFS)
	return_value = if_statement (scope, expression, &getif);
      else
	return_value = on_error (scope, expression, &getif);

      if (getif)
	/* If the conditional was successful, set the current depth's
	 * if_open to SUCCESS, so that any following 'else' clauses
	 * will not be run.
	 */
	if_open[depth] = OPEN_SUCCESS;
      else
	{
	  /* If the conditional was unsuccessful, set the current depth
	   * to a failure and procede to loop through the body and mark
	   * all inset if/on_error statements as failures (so their
	   * corresponding else clauses will not be run.
	   */
	  if_open[depth] = OPEN_FAILED;
	  
	  for (index = 1, jndex = depth + 1; expression.syntax[index] != NULL; index++)
	    {
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
      return_value.v_point = alloc_var ();
    }
  else
    {
      /* Move expression back. */
      expression.syntax -= get_ip ();
      expression.lines  -= get_ip ();
      return_value = eval (scope, expression);
    }

  /* If we have gotten any signals, we set the return
   * value to the correct ones.
   */
  if (!return_value.return_signal)
    return_value.return_signal = return_signal;
  if (!return_value.break_signal)
    return_value.break_signal = break_signal;

  /* Decrease the recursion depth and exit. */
  depth--;
  return return_value;
}

FACT_t
procedure (func_t *scope, word_list expression)
{
  int    length;
  FACT_t return_value;
  unsigned long ip;

  increase_depth ();

  expression.syntax += get_ip ();
  expression.lines  += get_ip ();
  
  ip = 1;
  
  while (expression.syntax[0] != NULL
	 && tokcmp (expression.syntax[0], "}"))
    {
      reset_ip ();
      return_value = eval_expression (scope, expression);

      if (return_value.type == ERROR_TYPE
	  || return_value.return_signal
	  || return_value.break_signal)
	{
	  /* If we're in here, that means that we've caught
	   * a signal. We don't break, since that would
	   * malform the signal, we set the ip to after
	   * the closing '}' and return.
	   */
	  set_ip (ip + get_exp_length (expression.syntax, '}'));
	  return return_value;
	}

      length = get_exp_length_first (expression.syntax, ';');
      ip    += length + 1;
      expression.syntax += length;
      expression.lines  += length;
    }

  /* Close all open if/on_error blocks, set the signals and
   * return values to their defualts, and decrease the
   * recusion depth by one. Then, return.
   */
  close_blocks ();
  next_inst ();

  return_value.return_signal = false;
  return_value.break_signal  = false;

  return_value.v_point       = alloc_var ();
  return_value.type          = VAR_TYPE;

  depth--;
      
  return return_value;
}

FACT_t
lambda_proc (func_t *scope, word_list expression)
{
  func_t * new_scope;
  FACT_t   return_value;

  /* Here we make a new scope for the procedure. This allows
   * for a psuedo-lambda class, that doesn't take any aguments.
   */
  new_scope = alloc_func ();

  /* Set the new scope to the correct values. We assume every
   * other field is initialized to NULL already (except for
   * array_size, which would be initialized to one).
   */
  new_scope->line      = scope->line      ;
  new_scope->name      = scope->name      ;
  new_scope->file_name = scope->file_name ;
  new_scope->up        = scope            ;

  /* Here is where the procedure is actually run (see procedure). */
  
  return_value = procedure (new_scope, expression);
  scope->line  = new_scope->line;

  return return_value;
}

FACT_t
eval (func_t *scope, word_list expression)
{
  /* eval - evaluate one single instruction. Since this function
   * has no real control over the depth and if/on_error blocks,
   * it cannot handle statements (although there's no real reason
   * for this, so it might change in the future to be combined
   * with eval_expression).
   */
  int      index;
  int      call_num;
  char   * current_token;
  var_t  * hold_var;
  func_t * hold_func;
  FACT_t   return_value;
  unsigned long ip;
  unsigned long hold_pointer;
  unsigned byte bytes;

  /* Get the instruction pointer (ip), and then set the
   * current_token (using the ip). Then, move the ip one
   * to indicate that the token has been evaluated.  
   */
  ip = get_ip ();
  current_token = expression.syntax[ip];
  next_inst ();

  /* Assert some stuff. */
  assert (current_token != NULL);

  /* Get the number of newlines. */
  scope->line += (expression.lines != NULL) ? expression.lines[0] : 0;
  
  /* Check to see if the current token is valid bytecode or, 
   * a valid number, and if so, call the desired instruction
   * or get the desired var_t * pointer.
   */
  if (current_token[0] == BYTECODE)
    {
      /* check which category it's from */
      if (current_token[1] == MATH_CALL)
	return_value = eval_math (scope, expression, (int) current_token[2]);
      if (current_token[1] == PRIMITIVE)
	return_value = run_prim (scope, expression, (int) current_token[2]);
      else if (current_token[1] == NUMBER)
	{
	  /* This part is a little bit complicated to understand.
	   * The instructions for a number are spread out over
	   * eight or four bytes (depending on whether it's a 64
	   * or 32 bit system respectively) which are converted
	   * to an unsigned long which is then cast into a pointer.
	   * To quote Ken Thompson, you aren't expected to
	   * undertsand this.
	   */
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
      /* If it is not a bytecode instruction, then we
       * check to see if it's a variable or a function.
       * If it is neither, we return an un-catchable
       * error. We have to do this check every time
       * because variables/function cannot be known
       * for sure to exist. For example, one odd thing
       * a user can legitamitely do is conditionally
       * define differently named variables/functions.
       */
      if ((call_num = isprim (current_token)) != -1)
	return_value = run_prim (scope, expression, call_num);
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

  /* Depending on whether or not the return value is
   * a variable or a function, we check for a variable
   * array or a function array. This includes numbers,
   * values returned by primitives, etc. Believe me,
   * accessing array elements after a primitive is
   * actually incredible useful.
   */
  if (return_value.type == VAR_TYPE)
    return_value = get_array_var (return_value.v_point, scope, expression);
  else if (return_value.type == FUNCTION_TYPE)
    return_value = get_array_func (return_value.f_point, scope, expression);

  /* Since we are not at the start of an expression,
   * we cannot return a value or break from a loop.
   * Thus, we set the respective signals to off for
   * protection.
   */
  return_value.return_signal = false;
  return_value.break_signal  = false;
  
  return return_value;
}
