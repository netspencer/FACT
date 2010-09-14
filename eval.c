#include "interpreter.h"

a_type 
expression (func *scope, char **words)
{
  a_type return_value; /* value to be eventually returned if nothing fails */
  a_type error; /* value to be returned if there is an error */
  int position; /* current position in "words" */
  char **formatted_expression; /* the block to be evaluated */
  bool isreturn; /* if the block starts with a return or not */

  extern int get_exp_length_first (char **, int);

  isreturn = false;
  return_value.type = VAR_TYPE;
  error.type = ERROR_TYPE;
  error.error.function = "expression";
  error.error.scope = scope;
  
  position = get_exp_length_first (words, ';'); /* find out how long the statement is */

  if (position < 1
      || (words[position - 1][0] != ';'
	  && words[position - 1][0] != '}'))
    {
      error.error.error_code = SYNTAX;
      return error;
    }

  formatted_expression = (char **) better_malloc (sizeof (char *) * (position + 2)); /* allocate space for expression */

  for (formatted_expression[position + 1] = NULL; position >= 0; position--)
    formatted_expression[position] = words[position];

  if (strcmp (formatted_expression[0], "if") == 0)/*
						    Checks to see if it is an if of while statement a
						    This ensures that an if or while loop will have to
						    come at the beginning of the statement.
						  */
    {
      return_value = if_statement (scope, formatted_expression + 1);
    }
  else if (strcmp (formatted_expression[0], "while") == 0)
    {
      return_value = while_loop (scope, formatted_expression + 1);
    }
  else if (strcmp (formatted_expression[0], "{") == 0)
    {
      return_value = lambda_proc (scope, formatted_expression + 1);
    }
  else if (strcmp (formatted_expression[0], "return") == 0)
    {
      return_value = eval (scope, formatted_expression + 1);
      isreturn = true;
    }
  else
    {
      return_value = eval (scope, formatted_expression);
    }

  if (!return_value.isret)
    return_value.isret = isreturn;

  return return_value;
}

a_type
 procedure (func *scope, char **words)
{
  a_type return_value; /* value returned */
  a_type error;
  int len_to_move; /* length moved each time */

  error.type = ERROR_TYPE;
  error.error.function = "procedure";
  error.error.scope = scope;

  extern int get_exp_length_first (char **, int);

  while (*words != NULL && strcmp (*words, "}"))
    {
      len_to_move = get_exp_length_first (words, ';');
      return_value = expression (scope, words);

      if (return_value.type == ERROR_TYPE
	  || return_value.isret == true)
	return return_value;

      words += len_to_move;
    }
    
  if (*words == NULL || !strcmp(*words, "}"))
    {
      return_value.isret = false;
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
      "lambda",
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
  a_type error;

  error.type = ERROR_TYPE;
  error.error.function = "evaluator";
  error.error.scope = scope;

  if (words[0] == NULL)
    {
      error.error.error_code = SYNTAX;
      return error;
    }
  
  if (strcmp (words[0], "list") == 0)
    {
      scroll (scope);
      error.error.error_code = NON;
      return error;
    }
  else if (isnum (words[0]))
    {
      return_value = num_to_var (words[0]);
    }
  else if (isprim (words[0]) > -1)
    {
      return runprim (scope, words);
    }
  else if (ismathcall (words[0]) > -1)
    {
      return eval_math (scope, words);
    }
  else if (get_var (scope, words[0]) != NULL)
    return_value = get_array_var (get_var (scope, words[0]),
				  scope,
				  words + 1);
  else if (get_func (scope, words[0]) != NULL)
    return_value = get_array_func (get_func (scope, words[0]),
				   scope,
				   words + 1);
  /*
    {
      return_value.type = FUNCTION_TYPE;
      return_value.f_point = getFunc (scope, words[0]);
    }
  */
  else
    {
      printf ("Unknown word: [%s]\n", words[0]);
      error.error.error_code = SYNTAX;
      return error;
    } 

  return_value.isret = false;

  return return_value;
}
