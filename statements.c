#include "common.h"

a_type
invalid_if (func *scope, char **words)
{
  return errorman_throw_reg (scope, "invalid syntax, if statements must start at the beginning of the expression");
}

a_type
invalid_else (func *scope, char **words)
{
  return errorman_throw_reg (scope, "invalid syntax, else statements must follow if statements at the beginning of the expression");
}

a_type
invalid_while (func *scope, char **words)
{
  return errorman_throw_reg (scope, "invalid syntax, while loops must start at the beginning of the expression");
}

a_type
if_statement (func *scope, char **words, bool *success)
{
  a_type return_value;
  a_type conditional;
  /*int pos;
    int count;*/
  mpz_t zero;

  func temp_scope =
    {
      "if_temp",
      NULL,
      NULL,
      1,
      NULL,
      NULL,
      scope,
      NULL,
      NULL
    };
  
  mpz_init (zero);
  
  (*success) = true;
  
  if (strcmp (*words, "(") != 0)
    return errorman_throw_reg (scope, "expected '(' after if statement");

  conditional = eval (scope, words);

  if (conditional.type == ERROR_TYPE)
    {
      conditional.error.scope = scope;
      return conditional; //errorman_throw_reg (scope, conditional.error.description);
    }

  if (conditional.type == FUNCTION_TYPE)
    return errorman_throw_reg (scope, "if statement conditional must return a var");
  
  if (mpz_cmp (conditional.v_point->data, zero) == 0)
    {
      /*for (pos = 1, count = 0; words[pos] != NULL; pos++)
        {
          if (words[pos][0] == '{'
	      || words[pos][0] == '('
	      || words[pos][0] == '[')
            count++;
          else if (words[pos][0] == '}'
		   || words[pos][0] == ')'
		   || words[pos][0] == ']')
            count--;
          else if (strcmp (words[pos], "else") == 0 && count == 0)
            break;
        }

      if (count > 0)
	return errorman_throw_reg (scope, "syntax error in if statement");

      if (words[pos] == NULL || strcmp (words[pos], "else") != 0)
      { */
          return_value.v_point = alloc_var ();
	  return_value.type = VAR_TYPE;
	  return_value.isret = false;
	  return_value.break_signal = false;

	  (*success) = false;  
	  
          return return_value;
	  /*  }
      else
	{
	  return_value = expression (&temp_scope, (words + pos + 1));

	  if (return_value.type == ERROR_TYPE)
	    return errorman_throw_reg (scope, combine_strs ("error in if statement block; ",
							    return_value.error.description));

	  return return_value;
	  } */
    }
  
  return_value = expression (&temp_scope, (words + 1));

  if (return_value.type == ERROR_TYPE)
    return_value.error.scope = scope;//errorman_throw_reg (scope, return_value.error.description);
    
  return return_value;
}

a_type
else_clause (func *scope, char **words)
{
  a_type return_value;
  
  func temp_scope =
    {
      "if_temp",
      NULL,
      NULL,
      1,
      NULL,
      NULL,
      scope,
      NULL,
      NULL
    };

  return_value = expression (&temp_scope, words);

  if (return_value.type == ERROR_TYPE)
    return errorman_throw_reg (scope, return_value.error.description);

  return return_value;
}

a_type
while_loop (func *scope, char **words)
{
  int pos_cond;
  int pos_block;
  int pos;
  char **conditional_saved;
  char **block_saved;
  char **conditional_temp;
  char **block_temp;
  a_type conditional_evald;
  a_type block_evald;
  mpz_t zero;

  extern int get_exp_length_first (char **, int);

  if (words[0] == NULL || words[0][0] != '(')
    return errorman_throw_reg (scope, "expected '(' after while");

  pos_cond = get_exp_length (words + 1, ')');
  conditional_saved = (char **) better_malloc ((sizeof (char *)) * (pos_cond + 1));

  conditional_saved[pos_cond] = NULL;
  pos = pos_cond;

  while (pos > 0)
    {
      pos--;
      conditional_saved[pos] = words[pos + 1];
    }

  if (words[pos_cond] == NULL)
    return errorman_throw_reg (scope, "syntax error in while loop");

  pos_block = get_exp_length_first (words + pos_cond, ';');
  block_saved = (char **) better_malloc ((sizeof (char *)) * (pos_block + 2));

  block_saved[pos_block + 1] = NULL;
  pos = pos_block;

  while (pos > 0)
    {
      pos--;
      block_saved[pos] = words[pos + pos_cond + 1];
    }

  for (; words[pos_block + pos_cond + pos] != NULL; pos++)
    words[pos] = words[pos_block + pos_cond + pos];

  words[pos] = NULL;

  block_evald.type = VAR_TYPE;
  block_evald.v_point = alloc_var ();
  mpz_init (zero);

  for (;;)
    {
      conditional_temp = copy (conditional_saved);

      conditional_evald = eval (scope, conditional_temp);
      
      if (conditional_evald.type == ERROR_TYPE)
	return conditional_evald;/*errorman_throw_reg (scope, combine_strs ("error in while loop conditional; ",
		 conditional_evald.error.description));*/
      if (conditional_evald.type == FUNCTION_TYPE)
	return errorman_throw_reg (scope, "while loop conditional must return a var");

      if (mpz_cmp (conditional_evald.v_point->data, zero) == 0)
        break;

      block_temp = copy (block_saved);

      block_evald = expression (scope, block_temp);

      if (block_evald.type == ERROR_TYPE)
	return block_evald;/*errorman_throw_reg (scope, combine_strs ("error in while loop block; ",
		 block_evald.error.description));*/
      if (block_evald.isret == true)
        break;

      if (block_evald.break_signal == true)
	{
	  block_evald.break_signal = false;
	  break;
	}
    }

  return block_evald;
}


