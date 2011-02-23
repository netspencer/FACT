#include "FACT.h"

static char * error;

void
print_parsing_error (const char *file_name, int line_num)
{
  printf ("Parsing error in <%s> on line %d: %s.\n", file_name, line_num, error);
  error = NULL;
}

bool
check_for_incompletions (const char *file_name, char *expression)
{
  /**
   * check_for_incompletions: Loop through a string and check for
   * mismatched parentheses, brackets, and curly-braces. Return
   * true on a mismatch and print the errors. Otherwise, return 
   * false. Also checks for quotes.
   *
   * @file_name - name of the current file (used for printing errors).
   * @expression - pure, uncut and unparsed expression.
   */

  int  p_count;
  int  b_count;
  int  c_count;
  bool in_quote;


  for (p_count = b_count = c_count = 0, in_quote = false; // Yes I know false = 0
       *(expression + 1) != '\0'; expression++)
    {
      if (!in_quote)
	{
	  if (*expression == '(')
	    p_count++;
	  else if (*expression == ')')
	    p_count--;
	  else if (*expression == '[')
	    b_count++;
	  else if (*expression == ']')
	    b_count--;
	  else if (*expression == '{')
	    c_count++;
	  else if (*expression == '}')
	    c_count--;
	}
      if (*expression == '"')
	{
	  if (!in_quote)
	    in_quote = true;
	  else if (*(expression - 1) != '\\'
		   || *(expression - 2) == '\\')
	    in_quote = false;
	}
    }

  if (p_count)
    printf ("Parsing error in <%s>: %d cases of mismatched parentheses.\n", file_name, abs (p_count));
  if (b_count)
    printf ("Parsing error in <%s>: %d cases of mismatched brackets.\n", file_name, abs (b_count));
  if (c_count)
    printf ("Parsing error in <%s>: %d cases of mismatched curly-braces.\n", file_name, abs (c_count));
  if (in_quote)
    printf ("Parsing error in <%s>: unmatched quotation mark.\n", file_name);

  return (p_count || b_count || c_count);
}

bool
check_for_errors (linked_word *expression,
		  unsigned int level, int *current_line, bool break_ok)
{
  /**
   * Values for 'level':
   *  = 0: Expression, not inside anything.
   *  = 1: (, no commas aloud.
   *  = 2: [, no commas aloud.
   *  = 3: {, no commas aloud.
   *  = 4: (, commas aloud.
   *  = 5: ![, commas aloud.
   *  = 6: Define / defunc.
   *  = 7: For, comma can end.
   *  = 8: For, then can end.
   */
  bool         neg_prev;
  bool         hold_ret;
  unsigned int prev_code;

  enum
  {
    OP_START,
    NON_OP  ,
  };

 new_expression:

  if (level == 0 || level == 3)
    {
      if (expression->code == IF
	  || expression->code == ON_ERROR)
	{
	  *current_line += expression->newlines;
	  if (expression->next->code != OP_PAREN)
	    {
	      error = "expected parenthesis after if/on_error statement";
	      return true;
	    }
	  if (check_for_errors (expression->next->hidden, 1, current_line, false))
	    return true;
	  return check_for_errors (expression->next->next, level, current_line, break_ok);
	}
      
      if (expression->code == ELSE)
	{
	  *current_line += expression->newlines;
	  return check_for_errors (expression->next, 0, current_line, break_ok);
	}

      if (expression->code == WHILE)
	{
	  current_line += expression->newlines;
	  if (expression->next->code != OP_PAREN)
	    {
	      error = "expected parenthesis after while statement";
	      return true;
	    }
	  if (check_for_errors (expression->next->hidden, 1, current_line, false))
	    return true;
	  return check_for_errors (expression->next->next, level, current_line, true);
	}
      
      if (expression->code == FOR)
	{
	  *current_line += expression->newlines;
	  return check_for_errors (expression->next, 7, current_line, break_ok);
	}
    }

  neg_prev = false;
  for (prev_code = OP_START; expression != NULL && expression->code != END; expression = expression->next)
    {
      *current_line += expression->newlines;
      /* neg_prev starts out as false, when a negative sign is hit, neg_prev
       * is set to true for one iteration. Any minus signs caught during the
       * period when neg_prev is true will make the function return false
       * At the of the iteration, neg_prev is reset to false.
       */
      if (level == 6 && (expression->code != OP_BRACKET
			 && expression->code != UNKNOWN))
	{
	  error = "invalid syntax in definition";
	  return true;
	}
      
      switch (expression->code)
        {
        case OP_CURLY:
          if (prev_code == NON_OP)
            {
              error = "unexpected '{'";
              return false;
            }
          if (check_for_errors (expression->hidden, 3, current_line, break_ok))
            return true;

          if (level == 0 || level == 3)
            {
              prev_code = OP_START;
              level = 3;
              expression = expression->next;
              goto new_expression;
            }
          prev_code = NON_OP;
          break;
          
        case CL_CURLY:
          if (level != 3 || (expression->previous != NULL
                             && expression->previous->code != SEMI
                             && expression->previous->code != OP_CURLY))
            {
              error = (level != 3)
                ? "unexpected '}'"
                : "expected semicolon before closing curly-brace";
              return true;
            }
          return false;
          
        case CL_BRACKET:
	  if (prev_code == OP_START || (level != 2 && level != 5))
	    {
	      error = " unexpected ']'";
	      return true;
	    }
          return false;
          
        case CL_PAREN:
          if ((expression->previous != NULL
               && prev_code == OP_START) || (level != 1 && level != 4))
            {
              error = "unexpected ')'";
              return true;
            }
          return false;
          
        case SEMI:
          if (level != 0 && level != 3)
            {
              error = "unexpected ';'";
              return true;
            }
          if (level == 0 && expression->next->code == END)
            return false;
          level = 3;
          expression = expression->next;
          goto new_expression;

        case COMMA:
          if (level < 4 || level > 7 || level == 6
              || prev_code != NON_OP)
            {
              error = "unexpected ','";
              return true;
            }
          prev_code = OP_START;
          if (level == 7)
            level++;
          break;
          
        case THEN:
          if (level != 8 || prev_code != NON_OP)
            {
              error = "unexpected 'then'";
              return true;
            }
          return check_for_errors (expression->next, 0, current_line, true);
          
        case BREAK_SIG:
          if (!break_ok)
            {
              error = "break signals can only be within for or while loops";
              return true;
            }
          if (expression->next->code != END
              && expression->next->code != SEMI)
            {
            error = "break signals cannot alone in an expression";
            return true;
            }
          prev_code = NON_OP;
          break;
          
        case RETURN_STAT:
          if (prev_code == NON_OP)
            {
              error = "unexpected return statement";
              return true;
            }
          prev_code = OP_START;
          break;

        case OP_PAREN:
	  if (prev_code == NON_OP)
	    {
	      error = "unexpected '('";
	      return true;
	    }
	  if (check_for_errors (expression->hidden, 1, current_line, break_ok))
	    return true;
	  prev_code = NON_OP;
          break;

        case FUNC_RET:
        case FUNC_OBJ:
	  if (prev_code == NON_OP)
	    {
	      error = "unexpected function call"; 
	      return true;
	    }
	  if (expression->next->code != OP_PAREN)
	    {
	      error = "expected opening parenthesis after function call.\n";
	      return true;
	    }
	  if (check_for_errors (expression->next->hidden, 4, current_line, break_ok))
	    return true;
	  expression = expression->next;
	  prev_code  = NON_OP;
          break;

        case DEF:
        case DEFUNC:
	  if (prev_code == NON_OP)
	    {
	      error = "unexpected definition";
	      return true;
	    }
	  if (check_for_errors (expression->hidden, 6, current_line, break_ok))
	    return true;
	  prev_code = NON_OP;
          break;

        case OP_BRACKET:
          if (prev_code == OP_START && level != 6)
            {
              if (check_for_errors (expression->hidden, 5, current_line, break_ok))
                return true;
	    }
	  else
	    {
	      if (check_for_errors (expression->hidden, 2, current_line, break_ok))
		return true;
	    }
	  prev_code = NON_OP;
          break;

        case COMB_ARR:
        case PLUS:
        case MINUS:
        case MULTIPLY:
        case DIVIDE:
        case MOD:
        case NEG:
        case ADD_ASSIGN:
        case SUB_ASSIGN:
        case MULT_ASSIGN:
        case DIV_ASSIGN:
        case MOD_ASSIGN:
        case SET:
        case IN_SCOPE:
        case AND:
        case OR:
        case BIT_AND:
        case BIT_IOR:
        case BIT_XOR:
        case EQ:
        case NEQ:
        case LESS:
        case MORE:
        case LESS_EQ:
        case MORE_EQ:
	  if (prev_code == OP_START)
	    {
	      if (expression->code == MINUS && !neg_prev)
		{
		  neg_prev = true;
		  continue;
		}
	      error = combine_strs ("unexpected ", lookup_word (expression->code - COMB_ARR, 0)); 
	      return true;
	    }
	  prev_code = OP_START;
          break;
          
        case UNKNOWN:
        case QUOTE:
        case VARIADIC:
          if (prev_code == NON_OP && level != 6)
	    {
	      error = combine_strs ("unexpected ", (expression->code == QUOTE) ? "\"" : expression->physical);
	      return true;
	    }
	  prev_code = NON_OP;
          break;

        case AT:
	  if (prev_code == NON_OP)
	    {
	      error = "unexpected function setting";
	      return true;
	    }
	  expression = expression->next;
	  check_for_errors (expression, 0, current_line, break_ok);
	  expression = expression->next;
	  if (expression->code != OP_PAREN)
	    {
	      error = "expected '(' after function decleration";
	      return true;
	    }
	  if (check_for_errors (expression->hidden, 4, current_line, break_ok))
	    return true;
	  if (expression->next->code != OP_CURLY)
	    {
	      error = "expected '{' after argument declaration";
	      return true;
	    }
	  if (check_for_errors (expression->next->hidden, 3, current_line, break_ok))
	    return true;
	  prev_code = NON_OP;
          break;

        default:
          break;
        }
      neg_prev = false;
    }
  
  return false;
}      
