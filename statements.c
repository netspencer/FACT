#include "interpreter.h"

a_type
if_statement (func *scope, char **words)
{
  a_type return_value, error;
  a_type conditional;
  int pos;
  int count;
  mpz_t zero;

  error.type = ERROR_TYPE;
  error.error.function = "if_statement";
  error.error.scope = scope;

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
  
  if (strcmp (*words, "(") != 0)
    {
      error.error.error_code = SYNTAX;
      return error;
    }

  conditional = eval (scope, words);

  if (conditional.type != VAR_TYPE)
    {
      error.error.error_code = INVALPRIM;
      return error;
    }
  
  if (mpz_cmp (conditional.v_point->data, zero) == 0)
    {
      for (pos = 1, count = 0; words[pos] != NULL; pos++)
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
	{
	  error.error.error_code = SYNTAX;
	  return error;
	}

      if (words[pos] == NULL || strcmp (words[pos], "else") != 0)
        {
          return_value.v_point = alloc_var ();
	  return_value.type = VAR_TYPE;
	  return_value.isret = false;
          return return_value;
        }
      else
	{
	  return_value = expression (&temp_scope, (words + pos + 1));

	  if (return_value.type == ERROR_TYPE)
	    return_value.error.scope = scope;

	  return return_value;
	}
    }
  
  return_value = expression (&temp_scope, (words + 1));

  if (return_value.type == ERROR_TYPE)
    return_value.error.scope = scope;
  
  return return_value;
}

a_type while_loop (func *scope, char **words)
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
  a_type error;
  mpz_t zero;

  extern char **copy (char **);
  extern int get_exp_length_first (char **, int);

  error.type = ERROR_TYPE;
  error.error.function = "while";
  error.error.scope = scope;

  if (words[0] == NULL || words[0][0] != '(')
    {
      error.error.error_code = SYNTAX;
      return error;
    }

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
    {
      error.error.error_code = SYNTAX;
      return error;
    }

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
      if (conditional_evald.type != VAR_TYPE)
	{
	  error.error.error_code = INVALPRIM;
	  return error;
	}

      if (mpz_cmp (conditional_evald.v_point->data, zero) == 0)
        break;

      block_temp = copy (block_saved);

      block_evald = expression (scope, block_temp);

      if (block_evald.type == ERROR_TYPE || block_evald.isret == true)
        break;
    }

  return block_evald;
}


