#include "common.h"

#define OPEN_SUCCESS 2
#define OPEN_FAILED  1
#define CLOSED       0

#define MAX_RECURSION 500 

FACT_t
expression (func_t *scope, char **words)
{
  int           line;
  int           position;             /* current position in "words" */                
  int           hold;
  bool          isreturn;             /* if the block starts with a return or not */
  bool          isbreak;
  bool          getif;
  char       ** formatted_expression; /* the block to be evaluated */
  FACT_t        return_value;         /* value to be eventually returned if nothing fails */
  word_list     expression;
  static int    depth = -1;
  static int    ifopen [MAX_RECURSION];

  isreturn          = false;
  isbreak           = false;
  return_value.type = VAR_TYPE;   
  position          = get_exp_length_first (words, ';'); /* find out how long the statement is */

  depth++;

  if (depth == MAX_RECURSION)
    {
      depth--;
      return errorman_throw_reg (scope, "reached the maximum recursion depth");
    }
  if (position < 1
      || (tokcmp (words[position - 1], ";")
	  && tokcmp (words[position - 1], "}")))
      //(words[position - 1][line] != ';'
      //&& words[position - 1][line] != '}'))
    {
      depth--;
      return errorman_throw_reg (scope, "syntax error; expected closing ';' or '}'");
    }

  // Before:
  // formatted_expression    = better_malloc (sizeof (char *) * (position + 2)); /* allocate space for expression */
  // expression.move_forward = better_malloc (sizeof (bool  ) * (position + 2));
  // After:
  formatted_expression    = better_malloc (sizeof (char *) * (position + 1));
  expression.move_forward = better_malloc (sizeof (bool  ) * (position + 1));
  expression.lines        = better_malloc (sizeof (int   ) * (position + 1));

  for (formatted_expression[position] = NULL, position--; position >= 0; position--)
    {
      for (line = 0; words[position][line] == '\n'; line++);
      if (words[position] + line == '\0')
	formatted_expression[position] = NULL;
      else
	formatted_expression[position] = words[position] + line;
      expression.lines[position] = line;
    }

  scope->line += expression.lines[0];
      
  if (!tokcmp (formatted_expression[0], "if")
      || !tokcmp (formatted_expression[0], "on_error"))
    {
      expression.syntax = formatted_expression + 1;
      expression.move_forward++;
      expression.lines++;
      
      if (!tokcmp (formatted_expression[0], "if"))
	return_value = if_statement (scope, expression, &getif);
      else
	return_value = on_error (scope, expression, &getif);

      if (getif)
	ifopen[depth] = OPEN_SUCCESS;
      else
	{
	  ifopen[depth] = OPEN_FAILED;
	  for (position = 1; formatted_expression[position] != NULL; position++)
	    {
	      int temp_depth = depth + 1;
	      scope->line   += expression.lines[position];

	      if (temp_depth >= MAX_RECURSION)
		return errorman_throw_reg (scope, "You have too many nested if loops. They exceed the maximum recursion depth");
	      
	      if (!strcmp (formatted_expression[position], "if")
		  || !strcmp (formatted_expression[position], "on_error"))
		ifopen[temp_depth++] = OPEN_FAILED;
	      else if (!strcmp (formatted_expression[position], ";")
		       || !strcmp (formatted_expression[position], "}"))
		break;
	    }
	}
    }
  else if (tokcmp (formatted_expression[0], "else") == 0)
    {
      for (position = MAX_RECURSION - 1; position >= 0; position--)
	{
	  if (ifopen[position] == OPEN_FAILED)
	    {
	      hold         = depth;
	      depth        = position - 1;
	      return_value = else_clause (scope, formatted_expression + 1);
	      depth        = hold;
	      break;
	    }
	  else if (ifopen[position] == OPEN_SUCCESS)
	    {
	      /* Is see a major problem here. */
	      if (strcmp (formatted_expression[1], "if") != 0)
		ifopen[position] = CLOSED;
	      depth--;
	      return FACT_get_ui (0);
	    }
	}
      if (position == -1)
	{
	  depth--;
	  return errorman_throw_reg (scope, "unmatched 'else'");
	}
    }
  else
    {
      for (position = MAX_RECURSION - 1; position >= depth; position--)
	ifopen[position] = CLOSED;
       
      if (tokcmp (formatted_expression[0], "while") == 0)
	return_value = while_loop (scope, formatted_expression + 1);
      else if (tokcmp (formatted_expression[0], "for") == 0)
	return_value = for_loop (scope, formatted_expression + 1);
      else if (tokcmp (formatted_expression[0], "{") == 0)
	{
	  expression.syntax = formatted_expression + 1;
	  return_value      = lambda_proc (scope, expression);
	  for (position = depth + 1; position < MAX_RECURSION; position++) 
	    ifopen[position] = CLOSED;
	}
      else if (tokcmp (formatted_expression[0], "return") == 0)
	{
	  expression.syntax = formatted_expression + 1;
	  return_value      = eval (scope, expression);
	  isreturn          = true;
	}
      else if (tokcmp (formatted_expression[0], "break") == 0)
	isbreak = true;
      else
	{
	  scope->line      -= expression.lines[0];
	  expression.syntax = formatted_expression;
	  return_value      = eval (scope, expression);
	}
    }

  if (!return_value.isret)
    return_value.isret = isreturn;
  if (!return_value.break_signal)
    return_value.break_signal = isbreak;

  depth--;
  return return_value;
}

FACT_t
procedure (func_t *scope, char **words)
{
  int    len_to_move;  /* length moved each time */  
  FACT_t return_value; /* value returned */

  while (*words != NULL && tokcmp (*words, "}"))
    {
      len_to_move  = get_exp_length_first (words, ';');
      return_value = expression (scope, words);

      if (return_value.type == ERROR_TYPE
	  || return_value.isret
	  || return_value.break_signal)
	return return_value;

      words += len_to_move;
    }
    
  if (*words == NULL || !tokcmp(*words, "}"))
    {
      return_value.isret        = false;
      return_value.break_signal = false;
      return_value.v_point      = alloc_var ();
      return_value.type         = VAR_TYPE;
      
      return return_value;
    }
    
  return (expression (scope, ++words));
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

  return_value = procedure (&temp_local, expression.syntax);
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
  scope->line += (expression.lines != NULL) ? expression.lines[0] : 0;

  if (word == NULL)
    return errorman_throw_reg (scope, "cannot evaluate empty expression");

  /*
    something in here moving the newline # forward.
  */
  
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
  
  return_value.isret        = false;
  return_value.break_signal = false;

  return return_value;
}
