#include "common.h"

#define OPEN_SUCCESS 0
#define OPEN_FAILED  1
#define CLOSED       2

a_type 
expression (func *scope, char **words)
{
  a_type return_value; /* value to be eventually returned if nothing fails */
  int position; /* current position in "words" */
  char **formatted_expression; /* the block to be evaluated */
  bool isreturn; /* if the block starts with a return or not */
  bool isbreak;
  bool getif;

  static int depth = -1;
  static int ifopen[100];

  extern int get_exp_length_first (char **, int);

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

  formatted_expression = (char **) better_malloc (sizeof (char *) * (position + 2)); /* allocate space for expression */

  for (formatted_expression[position + 1] = NULL; position >= 0; position--)
    formatted_expression[position] = words[position];

  if (strcmp (formatted_expression[0], "if") == 0)
    {
      return_value = if_statement (scope, formatted_expression + 1, &getif);

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
	  //	  ifopen[depth] = CLOSED;
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
  else if (strcmp (formatted_expression[0], "while") == 0)
    {
      ifopen[depth] = CLOSED;
      return_value = while_loop (scope, formatted_expression + 1);
    }
  else if (strcmp (formatted_expression[0], "{") == 0)
    {
      ifopen[depth] = CLOSED;
      return_value = lambda_proc (scope, formatted_expression + 1);
    }
  else if (strcmp (formatted_expression[0], "return") == 0)
    {
      ifopen[depth] = CLOSED;
      return_value = eval (scope, formatted_expression + 1);
      isreturn = true;
    }
  else if (strcmp (formatted_expression[0], "break") == 0)
    {
      ifopen[depth] = CLOSED;
      isbreak = true;
    }
  else
    {
      ifopen[depth] = CLOSED;
      return_value = eval (scope, formatted_expression);
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
  a_type return_value; /* value returned */
  int len_to_move; /* length moved each time */

  extern int get_exp_length_first (char **, int);

  while (*words != NULL && strcmp (*words, "}"))
    {
      len_to_move = get_exp_length_first (words, ';');
      return_value = expression (scope, words);

      if (return_value.break_signal)
	printf ("WWWWWWWWWWHOOO\n");

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
lambda_proc (func *scope, char **words)
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

  return procedure (&temp_local, words);
}

a_type
eval (func *scope, char **words)
{
  a_type return_value;

  if (words[0] == NULL)
    return errorman_throw_reg (scope, "cannot evaluate empty expression");
  
  if (strcmp (words[0], "list") == 0)
    {
      scroll (scope);
      return_value = num_to_var ("1");
    }
  else if (isnum (words[0]))
    return_value = num_to_var (words[0]);
  else if (isprim (words[0]) > -1)
    return runprim (scope, words);
  else if (ismathcall (words[0]) > -1)
    return eval_math (scope, words);
  else if (get_var (scope, words[0]) != NULL)
    return_value = get_array_var (get_var (scope, words[0]),
				  scope,
				  words + 1);
  else if (get_func (scope, words[0]) != NULL)
    return_value = get_array_func (get_func (scope, words[0]),
				   scope,
				   words + 1);
  else
    return errorman_throw_reg (scope, combine_strs ("cannot evaluate ",
						    words[0]));
  
  return_value.isret = false;
  return_value.break_signal = false;

  return return_value;
}
