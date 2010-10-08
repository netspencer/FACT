#include "common.h"

#define OPEN_SUCCESS 2
#define OPEN_FAILED  1
#define CLOSED       0

void
set_array (bool *rop, int op)
{
  int pos;

  for (pos = 0; pos < op; pos++)
    rop[pos] = false;
}

a_type 
expression (func *scope, char **words)
{
  int position; /* current position in "words" */
  
  bool isreturn; /* if the block starts with a return or not */
  bool isbreak;
  bool getif;
  
  char **formatted_expression; /* the block to be evaluated */

  a_type return_value; /* value to be eventually returned if nothing fails */

  word_list expression;

  static int depth = -1;
  static int ifopen[100];

  /*extern int get_exp_length_first (char **, int);*/

  isreturn = false;
  isbreak = false;

  return_value.type = VAR_TYPE;

  depth++;
  
  position = get_exp_length_first (words, ';'); /* find out how long the statement is */

  if (depth == 100)
    {
      depth--;
      return errorman_throw_reg (scope, "reached the maximum recursion depth of 100");
    }
  
  if (position < 1
      || (words[position - 1][0] != ';'
	  && words[position - 1][0] != '}'))
    {
      depth--;
      return errorman_throw_reg (scope, "syntax error; expected closing ';' or '}'");
    }

  formatted_expression = better_malloc (sizeof (char *) * (position + 2)); /* allocate space for expression */
  /* probably would be a good idea to move these two to be called only when they're
     needed. */
  expression.move_forward = better_malloc (sizeof (bool) * (position + 2));
  set_array (expression.move_forward, position + 2);

  for (formatted_expression[position + 1] = NULL; position >= 0; position--)
    formatted_expression[position] = words[position];

  if (strcmp (formatted_expression[0], "if") == 0)
    {
      expression.syntax = formatted_expression + 1;
      return_value = if_statement (scope, expression, &getif);

      if (getif)
	ifopen[depth] = OPEN_SUCCESS;
      else
	ifopen[depth] = OPEN_FAILED;
    }
  else if (strcmp (formatted_expression[0], "else") == 0)
    {
      if (ifopen[depth] == OPEN_FAILED)
	{
	  depth--;
	  return_value = else_clause (scope, formatted_expression + 1);
	  depth++;
	}
      else if (ifopen[depth] == OPEN_SUCCESS)
	{
	  if (strcmp (formatted_expression[1], "if") != 0)
	    ifopen[depth] = CLOSED;
	}
      else
	{
	  depth--;
	  return errorman_throw_reg (scope, "unmatched 'else'");
	}
    }
  else
    {
      ifopen[depth] = CLOSED;
       
      if (strcmp (formatted_expression[0], "while") == 0)
	return_value = while_loop (scope, formatted_expression + 1);
      else if (strcmp (formatted_expression[0], "for") == 0)
	return_value = for_loop (scope, formatted_expression + 1);
      else if (strcmp (formatted_expression[0], "{") == 0)
	{
	  expression.syntax = formatted_expression + 1;
	  return_value = lambda_proc (scope, expression);
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

a_type
procedure (func *scope, char **words)
{
  int len_to_move; /* length moved each time */
  
  a_type return_value; /* value returned */

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

a_type
lambda_proc (func *scope, word_list expression)
{
  func temp_local = 
    {
      scope->name,
      NULL,
      NULL,
      1,
      NULL,
      NULL,
      scope,
      NULL,
      NULL,
    }; /*
	 this is a temporary scope created to contain
	 lambda procedures. Note, these are different
	 from lambda functions. A lot.
       */
  
  return procedure (&temp_local, expression.syntax);
}

a_type
eval (func *scope, word_list expression)
{
  int call_num;

  char *word;
  
  a_type return_value;

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

      //      expression.move_forward[0] = true;
      
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
