#include "common.h"

#define OPEN_SUCCESS 2
#define OPEN_FAILED  1
#define CLOSED       0

#define MAX_RECURSION 500 

FACT_t
expression (func_t *scope, char **words)
{
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

  isreturn = false;
  isbreak = false;
  return_value.type = VAR_TYPE;

  depth++;
  
  position = get_exp_length_first (words, ';'); /* find out how long the statement is */

  if (depth == MAX_RECURSION)
    {
      depth--;
      return errorman_throw_reg (scope, "reached the maximum recursion depth");
    }  
  if (position < 1
      || (words[position - 1][0] != ';'
	  && words[position - 1][0] != '}'))
    {
      depth--;
      return errorman_throw_reg (scope, "syntax error; expected closing ';' or '}'");
    }

  formatted_expression = better_malloc (sizeof (char *) * (position + 2)); /* allocate space for expression */
  expression.move_forward = better_malloc (sizeof (bool) * (position + 2));

  for (formatted_expression[position + 1] = NULL; position >= 0; position--)
    formatted_expression[position] = words[position];

  if (strcmp (formatted_expression[0], "if") == 0)
    {
      expression.syntax = formatted_expression + 1;
      return_value = if_statement (scope, expression, &getif);

      if (getif)
	ifopen[depth] = OPEN_SUCCESS;
      else
	{
	  ifopen[depth] = OPEN_FAILED;
	  for (position = 1; formatted_expression[position] != NULL; position++)
	    {
	      int temp_depth = depth + 1;

	      if (temp_depth >= MAX_RECURSION)
		return errorman_throw_reg (scope, "You have too many nested if loops. They exceed the maximum recursion depth");
	      
	      if (!strcmp (formatted_expression[position], "if"))
		ifopen[temp_depth++] = OPEN_FAILED;
	      else if (!strcmp (formatted_expression[position], ";")
		       || !strcmp (formatted_expression[position], ";"))
		break;
	    }
	}
    }
  else if (strcmp (formatted_expression[0], "else") == 0)
    {
      for (position = MAX_RECURSION - 1; position >= 0; position--)
	{
	  if (ifopen[position] == OPEN_FAILED)
	    {
	      hold = depth;
	      depth = position - 1;
	      return_value = else_clause (scope, formatted_expression + 1);
	      depth = hold;
	      break;
	    }
	  else if (ifopen[position] == OPEN_SUCCESS)
	    {
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
       
      if (strcmp (formatted_expression[0], "while") == 0)
	return_value = while_loop (scope, formatted_expression + 1);
      else if (strcmp (formatted_expression[0], "for") == 0)
	return_value = for_loop (scope, formatted_expression + 1);
      else if (strcmp (formatted_expression[0], "{") == 0)
	{
	  expression.syntax = formatted_expression + 1;
	  return_value = lambda_proc (scope, expression);
	  for (position = depth + 1; position < MAX_RECURSION; position++) 
	    ifopen[position] = CLOSED;
	}
      else if (strcmp (formatted_expression[0], "return") == 0)
	{
	  expression.syntax = formatted_expression + 1;
	  return_value = eval (scope, expression);
	  isreturn = true;
	}
      else if (strcmp (formatted_expression[0], "break") == 0)
	isbreak = true;
      else
	{
	  expression.syntax = formatted_expression;
	  return_value = eval (scope, expression);
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

  while (*words != NULL && strcmp (*words, "}"))
    {
      len_to_move = get_exp_length_first (words, ';');
      return_value = expression (scope, words);

      if (return_value.type == ERROR_TYPE
	  || return_value.isret
	  || return_value.break_signal)
	return return_value;

      words += len_to_move;
    }
    
  if (*words == NULL || !strcmp(*words, "}"))
    {
      return_value.isret = false;
      return_value.break_signal = false;
      return_value.v_point = alloc_var ();
      return_value.type = VAR_TYPE;
      
      return return_value;
    }
    
  return (expression (scope, ++words));
}

FACT_t
lambda_proc (func_t *scope, word_list expression)
{
  func_t temp_local = 
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
  
   /*
     temp_local is a temporary scope created to contain
     lambda procedures. Note, these are different from
     lambda functions. A lot.
   */

  return procedure (&temp_local, expression.syntax);
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
    }

  word = expression.syntax[0];

  expression.move_forward[0] = true;

  if (word == NULL)
    return errorman_throw_reg (scope, "cannot evaluate empty expression");
  
  if (strcmp (word, "list") == 0)
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
  
  return_value.isret = false;
  return_value.break_signal = false;

  return return_value;
}
