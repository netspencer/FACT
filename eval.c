#include "common.h"

#define OPEN_SUCCESS 2
#define OPEN_FAILED  1
#define CLOSED       0

#define MAX_RECURSION 500

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
      if (index < 1
	  || (tokcmp (words[index - 1], ";")
	      && tokcmp (words[index - 1], "}")))
	{
	  expression.syntax = NULL;
	  return expression;
	}
    }
  else
    {
      /* ---- Get the length until the NULL terminator ---- */
      for (index = 0; words[index] != NULL; index++);
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
  bool   return_signal;
  bool   break_signal;
  bool   getif;
  FACT_t return_value;
  static int depth = -1;
  static int if_open [MAX_RECURSION];

#define BREAK_SIG 0 /* This number is arbitrary, and as long as it's different from
		       IF, ON_ERROR, ELSE, WHILE, FOR, and RETURN_STAT then it will
		       work. */
#define STAT_SIZE (((sizeof (statements)) / (sizeof (statements[0]))) - 1)

  /*
    The reason why I'm using a struct instead of just an array of
    char pointers is simply because this makes the interface a fair
    amount cleaner.
  */
  static struct
  {
    int    id;
    char * token;
  } statements [] =
      {
	{ IF         , "if"       },
	{ ON_ERROR   , "on_error" },
	{ ELSE       , "else"     },
	{ WHILE      , "while"    },
	{ FOR        , "for"      },
	{ OP_CURLY   , "{"        },
	{ RETURN_STAT, "return"   },
	{ BREAK_SIG  , "break"    },
	{ UNKNOWN    , NULL       }, /* This ends the struct, again used for interface
					reasons. */
      };

  /* ---- Set the default return_value type ----
     This probably isn't necessary. */
  return_value.type = VAR_TYPE;
  /* ---- Set the signals to their defaults ---- */
  return_signal = false;
  break_signal  = false;
  /* ---- Increase the recursion depth ---- */
  depth++;
  /* ---- Check the recursion depth ---- */
  if (depth == MAX_RECURSION)
    {
      depth--;
      return errorman_throw_reg (scope, "reached the maximum recursion depth");
    }
  /* ---- Check for an invalid statement ----
     Note: This will be removed in the future
     when I fix the parser's error checker. */
  if (expression.syntax == NULL) 
    {
      depth--;
      return errorman_throw_reg (scope, "syntax error: expected closing ';' or '}'");
    }
  /* ---- Search for a statement ---- */
  for (index = 0; index < STAT_SIZE; index++)
    {
      if (!tokcmp (statements[index].token, expression.syntax[0]))
	break;
    }
  /* ---- Do some things if it is a statement ---- */
  if (index < STAT_SIZE)
    {
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
      for (jndex = MAX_RECURSION - 1; jndex >= depth; jndex--)
	if_open[jndex] = CLOSED;
    }
  /* ---- If it's an if or an on_error statement ---- */
  if (statements[index].id == IF)
    {
      /* ---- Check if it's an on_error or if statement ---- */
      if (!tokcmp (expression.syntax[-1], "if"))
	return_value = if_statement (scope, expression, &getif);
      else
	return_value = on_error (scope, expression, &getif);

      if (getif)
	if_open[depth] = OPEN_SUCCESS;
      else
	{
	  if_open[depth] = OPEN_FAILED;
	  for (index = 1; expression.syntax[index] != NULL; index++)
	    {
	      int temp_depth = depth + 1;
	      scope->line   += expression.lines[index];

	      if (temp_depth >= MAX_RECURSION)
		return errorman_throw_reg (scope, "You have too many nested if loops. They exceed the maximum recursion depth");

	      if (!tokcmp (expression.syntax[index], "\""))
		{
		  if (expression.syntax[index + 2] != NULL)
		    scope->line += expression.lines[index + 2];
		  scope->line += expression.lines[index + 1];
		  index       += 2;
		}

	      if (!strcmp (expression.syntax[index], "if")
		  || !strcmp (expression.syntax[index], "on_error"))
		if_open[temp_depth++] = OPEN_FAILED;
	      else if (!strcmp (expression.syntax[index], ";")
		       || !strcmp (expression.syntax[index], "{"))
		break;
	    }
	}
    }
  else if (statements[index].id == ELSE)
    {
      for (index = MAX_RECURSION - 1; index >= 0; index--)
	{
	  int temp_depth;

	  if (if_open[index] == OPEN_FAILED)
	    {
	      temp_depth   = depth;
	      depth        = index - 1;
	      return_value = else_clause (scope, expression);
	      depth        = temp_depth;
	      break;
	    }
	  else if (if_open[index] == OPEN_SUCCESS)
	    {
	      if_open[index] = CLOSED;
	      temp_depth     = index;
	      for (index = 0; expression.syntax[index] != NULL; index++)
		{
		  scope->line += expression.lines[index];

		  if (temp_depth >= MAX_RECURSION)
		    return errorman_throw_reg (scope, "You have too many nested if loops. They exceed the maximum recursion depth");

		  if (!tokcmp (expression.syntax[index], "\""))
		    {
		      if (expression.syntax[index + 2] != NULL)
			scope->line += expression.lines[index + 2];
		      scope->line += expression.lines[index + 1];
		      index       += 2;
		    }

		  if (!tokcmp (expression.syntax[index], "if")
		      || !tokcmp (expression.syntax[index], "on_error"))
		    if_open[temp_depth++] = OPEN_SUCCESS;
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
      for (index = depth + 1; index < MAX_RECURSION; index++)
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
  FACT_t return_value; 

  while (expression.syntax[0] != NULL
	 && tokcmp (expression.syntax[0], "}"))
    {
      
      length_to_move = get_exp_length_first (expression.syntax, ';');
      return_value   = eval_expression (scope, expression);

      if (return_value.type == ERROR_TYPE
	  || return_value.return_signal
	  || return_value.break_signal)
	return return_value;

      expression.move_forward += length_to_move;
      expression.syntax       += length_to_move;
      expression.lines        += length_to_move;
    }

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
  
   /*
     temp_local is a temporary scope created to contain
     lambda procedures. Note, these are different from
     lambda functions. A lot.
   */
  
  return_value = procedure (&temp_local, expression);
#ifdef DEBUG
  printf ("scope: %d, local: %d\n", scope->line, temp_local.line);
#endif
  scope->line  = temp_local.line;

  return return_value;
}

FACT_t
eval (func_t *scope, word_list expression)
{
  int      call_num;
  char   * word;
  FACT_t   return_value;

  while (expression.move_forward[0])
    {
      expression.syntax++;
      expression.move_forward++;
      expression.lines++;
    }

  word                       = expression.syntax[0];
  expression.move_forward[0] = true;
  scope->line               += (expression.lines != NULL) ? expression.lines[0] : 0;

  if (word == NULL)
    return errorman_throw_reg (scope, "cannot evaluate empty expression");
  
  if (tokcmp (word, "list") == 0)
    {
      scroll (scope);
      return_value = num_to_var ("1");
    }
  else if (isnum (word))
    return_value = num_to_var (word);
  else
    {
      expression.syntax++;
      expression.move_forward++;
      expression.lines++;
      
      if ((call_num = isprim (word)) > -1)
	return runprim (scope, expression, call_num);
      else if ((call_num = ismathcall (word)) > -1)
	return eval_math (scope, expression, call_num);
      else if (get_var (scope, word) != NULL)
	return_value = get_array_var (get_var (scope, word), scope, expression);
      else if (get_func (scope, word) != NULL)
	return_value = get_array_func (get_func (scope, word), scope, expression);
      else
	return errorman_throw_reg (scope, combine_strs ("cannot evaluate ", word));
    }
  
  return_value.return_signal = false;
  return_value.break_signal  = false;

  return return_value;
}
