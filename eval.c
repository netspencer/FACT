#include "FACT.h"

word_list
make_word_list (char **words, bool len_check)
{
  /* To be removed because it is really bad. */
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

  expression.syntax = better_malloc (sizeof (char *) * (index + 1));
  expression.lines  = better_malloc (sizeof (int   ) * (index + 1));

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

//////////////////////////////////////////
// Instruction pointer accessor methods.
//////////////////////////////////////////

unsigned long * 
get_ip_ref (void)
{
  /**
   * get_ip_ref - return a pointer to the current thread's instruction
   * pointer. Completely reentrant.
   */
  unsigned long curr_tid; // Current thread id.

  curr_tid = FACT_get_tid ();
  assert (curr_tid != -1);
  return &(threads[curr_tid].ip);
}

unsigned long
get_ip (void)
{
  return *(get_ip_ref ()); // We just derefrence get_ip_ref.
}

void
set_ip (unsigned long nip) // Set the current ip to nip.
{
  *(get_ip_ref ()) = nip;
}

void
move_ip (unsigned long mip) // Move over the current ip by mip.
{
  *(get_ip_ref ()) += mip;
}

void
next_inst (void) // Move the current ip over by one.
{
  ++*(get_ip_ref ());
}

//////////////////////////
// Evaluation functions. 
//////////////////////////

static unsigned long
get_else (char ** expression)
{
  /**
   * get_else - get the length to the else clause corresponding 
   * to the given if statement. If none is available, return
   * the length to NULL.
   *
   * @ifs: how many if/on_error statements needed to be matched.  
   */

  unsigned long ifs   ; 
  unsigned long index ;

  for (index = ifs = 0; expression[index] != NULL; index++)
    {
      if (expression[index][0] == BYTECODE
	  && expression[index][1] == STATEMENT)
	{
	  /* Don't even touch ifs or elses if the token isn't
	   * bytecode and it isn't a statement.
	   */
	  switch (expression[index][2])
	    {
	    case IFS: // IFS stands for IF Statement.
	    case ONE: // ONE stands for ON Error.
	      ifs++; 
	      break;
	      
	    case ELS: // And ELS stands for Else clause.
	      if (--ifs == 0)
		return index; 
	      break;
	      
	    default:
	      break;
	    }
	}
      else
	{
	  switch (expression[index][0])
	    {
	    case ';':
	      if (expression[index + 1] == NULL
		  || expression[index + 1][0] != BYTECODE
		  || expression[index + 1][1] != STATEMENT
		  || expression[index + 1][2] != ELS)
		return index + 1;
	      break;
	      
	    case '{':
	      index += get_exp_length (expression + index + 1,'}');

	      if (expression[index + 1] == NULL
		  || expression[index + 1][0] != BYTECODE
		  || expression[index + 1][1] != STATEMENT
		  || expression[index + 1][2] != ELS)
		return index + 1;
	      break;

	    case '(':
	      index += get_exp_length (expression + index + 1, ')');
	      break;

	    case '[':
	      index += get_exp_length (expression + index + 1, ']');
	      break;
	      
	    default:
	      break;
	    }
	}
    }

  return index;
}

FACT_t
eval_expression (func_t *scope, word_list expression)
{  
  /**
   * eval_expression - checks the begining of the expression 
   * for statements such as if, while, or etc, and for signals
   * such as break or return.
   *
   * @scope: scope to evaluate the expression in.
   * @expression: expression to evaluate.
   */
  int    i;
  byte   instruction;
  bool   getif;
  bool   break_signal;
  bool   return_signal;
  bool   is_instruction;
  FACT_t return_value;

  /* Init routines -- Set the default return value to var,
   * set the signals to their default off, and increase
   * the recursion depth by one. 
   */
  return_value.type = VAR_TYPE;  
  return_signal = false;
  break_signal  = false;

  /* Move the current token to the instruction pointer. */
  expression.syntax += get_ip ();
  expression.lines  += get_ip ();

  /* This will be removed in the future. */
  if (expression.syntax == NULL) 
    return errorman_throw_reg (scope, "syntax error: expected closing ';' or '}'");

  while (expression.syntax[0][0] == BYTECODE
         && expression.syntax[0][1] == IGNORE) // Skip all of the ignores.
    {
      expression.syntax++;
      expression.lines++;
      next_inst ();
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

      /* Increse the current line number and instruction pointer
       * by one. This is so statements don't re-evaluated their
       * identifying tokens.
       */
      scope->line += expression.lines[0];
      next_inst ();
    }
  else
    is_instruction = false;

  /* The if and on_error statements are combined because
   * they are so similar. After the call their respective
   * functions, they do the exact same thing to the
   * if_open array.
   */
  if (is_instruction)
    {
      switch (instruction)
        {
        case IFS:
        case ONE:
          if (instruction == IFS)
            return_value = if_statement (scope, expression, &getif);
          else
            return_value = on_error (scope, expression, &getif);
          
          if (!getif)
            {
              /* If the conditional evaluated to false, get the place
               * of the else matching this current if statement, and
               * evaluate its expression.
               */
              i = get_else (expression.syntax);
              
              /* If there's no corresponding else statement, just
               * return whatever if_statement or on_error gave us.
               */
              if (expression.syntax[i] == NULL
                  || expression.syntax[i][0] != BYTECODE
                  || expression.syntax[i][1] != STATEMENT
                  || expression.syntax[i][2] != ELS)
                return return_value;
              
              expression.syntax += i;
              expression.lines  += i;
              
              return_value = else_clause (scope, expression);
            }
          break;

        case ELS:
          /* If it's an else clause, someone messed up, as those
           * are never called directly. Therefor, we return an
           * "unmatched else" error.
           */
          return errorman_throw_reg (scope, "unmatched 'else'");

        case WHL:
          return_value = while_loop (scope, expression);
          break;

        case FRL:
          return_value = for_loop (scope, expression);
          break;

        case RTN:
          set_ip (1);
          return_value  = eval (scope, expression);
          return_signal = true;
          break;

        case BRK:
          if (expression.syntax[0] != NULL
              && tokcmp (expression.syntax[0], ";"))
            return errorman_throw_reg (scope, "break must be alone in an expression");
          break_signal = true;
          return_value.v_point = alloc_var ();
          break;

        default:
          break;
        }
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

  return return_value;
}

FACT_t
procedure (func_t *scope, word_list expression)
{
  int           length;
  FACT_t        return_value;
  unsigned long ip;

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
	  set_ip (get_exp_length (expression.syntax, '}') + 1);
	  return return_value;
	}

      if (expression.syntax[0][0] == BYTECODE
	  && expression.syntax[0][1] == STATEMENT
	  && (expression.syntax[0][2] == IFS
	      || expression.syntax[0][2] == ONE))
	{
	  /* Using get_else will move the expression the exact
	   * correct amount forward, skipping all else expressions
	   * that are not to be evaluated. We just have to move
	   * forward one expression after.
	   */
	  length = get_else (expression.syntax);

	  while (expression.syntax[length] != NULL
                 && expression.syntax[length][0] == BYTECODE
		 && expression.syntax[length][1] == STATEMENT
		 && expression.syntax[length][2] == ELS)
	    length += get_else (expression.syntax + length + 1) + 1;
	}
      else
	length = get_exp_length_first (expression.syntax, ';');

      ip += length + 1;
 
      expression.syntax += length;
      expression.lines  += length;
    }

  next_inst ();

  return_value.return_signal = false;
  return_value.break_signal  = false;

  return_value.v_point       = alloc_var ();
  return_value.type          = VAR_TYPE;
      
  return return_value;
}

/* When ignore_signals is set to true, it overides the default
 * setting for eval to reset the return and break value to false.
 * This will be removed in the future. This variable is ONLY used
 * by lambda_proc and eval.
 */
static bool ignore_signals;

FACT_t
lambda_proc (func_t *scope, word_list expression)
{
  FACT_t return_value;
  func_t *new_scope;

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

  ignore_signals = true;

  return return_value;
}

FACT_t
eval (func_t * scope, word_list expression)
{
  /**
   * eval - evaluate one single instruction or return a variable
   * or a function.
   *
   * @scope:      The scope to evalute the instruction in.
   * @expression: Word list that contains every instruction in
   *              the expression.
   */
  int    i;
  int    call_num;
  bool   hold_break_sig;
  bool   hold_return_sig;
  FACT_t return_value;
  
  unsigned long ip;
  unsigned long hold_pointer;

  char   *current_token;
  var_t  *hold_var;
  func_t *hold_func;
  
  static unsigned byte bytes ;

  /* Get the instruction pointer (ip), and then set the
   * current_token (using the ip). Then, move the ip one
   * to indicate that the token has been evaluated.  
   */
  ip = get_ip ();

  while (expression.syntax[ip][0] == BYTECODE
         && expression.syntax[ip][1] == IGNORE) // Skip all of the ignores.
    {
      ip++;
      next_inst ();
    }

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
      /* If bytes isn't set, set it. */
      if (!bytes)
	bytes = (sizeof (var_t *) / sizeof (char));
      
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
	   */
	  return_value.type = VAR_TYPE;
	  hold_pointer = 0;
	  
	  for (i = 0; i < bytes; i++)
	    {
#if (BYTE_ORDER == LITTLE_ENDIAN)
	      hold_pointer <<= 8;
#else /* We assume big and little are the only values available */
	      hold_pointer >>= 8;
#endif
	      hold_pointer += (unsigned char) current_token[i + 2];
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

  /* Hold the return signals if ignore_signals is true.
   * Otherwise, set them to false.
   */
  if (ignore_signals)
    {
      hold_break_sig  = return_value.break_signal;
      hold_return_sig = return_value.return_signal;
      ignore_signals  = false;
    }
  else
    {
      hold_break_sig  = false;
      hold_return_sig = false;
    }

  // we skip all ignores so that other functions don't have to deal with them.
  for (ip = get_ip (); (expression.syntax[ip] != NULL
                        && (expression.syntax[ip][0] == BYTECODE
                            && expression.syntax[ip][1] == IGNORE)); ip++) 
    next_inst ();
      
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
  return_value.return_signal = hold_return_sig;
  return_value.break_signal  = hold_break_sig;

  return return_value;
}
