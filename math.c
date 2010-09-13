#include "interpreter.h"

/*---------------------------------------------*
 * math.c: Provides functions for various math *
 * procedures and other things.                *
 *                                             *
 * This code is (C) 2010 Matthew Plant.        *
 *---------------------------------------------*/

/* * * * * * * * * * * * * * * * * * * * * * * *
 * isnum: simple function that returns true if *
 * a given string is a valid number, and false *
 * otherwise.                                  *
 * * * * * * * * * * * * * * * * * * * * * * * */

bool isnum (char *word)
{
  int pos;

  for (pos = 0; word[pos] != '\0'; pos++)
    {
      if (!isdigit ((int) word[pos]))
	  return false;
    }

  return true;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * num_to_var: really simple function that     *
 * takes in a string and converts it to a      *
 * variable type wrapped in an ambigious type. *
 * * * * * * * * * * * * * * * * * * * * * * * */

a_type num_to_var (char *word)
{
  a_type return_value;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();
  
  mpz_set_str (return_value.v_point->data, word, 10);
  
  return_value.v_point->array_size = 1;
  return_value.v_point->array_up = NULL;
  return_value.v_point->next = NULL;
  
  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * add: function that adds two given           *
 * variables and returns the result, or an     *
 * error if the second variable is missing.    *
 * * * * * * * * * * * * * * * * * * * * * * * */

a_type add (a_type arg1, a_type arg2)
{
  a_type return_value, error;
  func *scope;

  scope = NULL;
  
  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();
  
  error.type = ERROR_TYPE;
  error.error.function = "add";
  error.error.scope = scope;
  
  if (arg2.type != VAR_TYPE)
    {
      error.error.error_code = INVALPRIM;
      return error;
    }
  
  mpz_add (return_value.v_point->data, arg1.v_point->data, arg2.v_point->data);
  
  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * sub: function that subtracts two given      *
 * variables and returns the result. This      *
 * function when called will never return an   *
 * error.                                      *
 * * * * * * * * * * * * * * * * * * * * * * * */

a_type sub (a_type arg1, a_type arg2)
{
  a_type return_value;
  func *scope;

  scope = NULL;

  return_value.type = VAR_TYPE;  
  return_value.v_point = alloc_var ();
  
  if (arg2.type != VAR_TYPE)
    mpz_neg (return_value.v_point->data, arg1.v_point->data);
  else
    mpz_sub (return_value.v_point->data, arg1.v_point->data, arg2.v_point->data);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * mult: function that multiplies two given    *
 * variables and returns the result. Returns   *
 * errors in the same cases as "add."          *
 * * * * * * * * * * * * * * * * * * * * * * * */

a_type mult (a_type arg1, a_type arg2)
{
  a_type return_value, error;
  func *scope;

  scope = NULL;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  error.type = ERROR_TYPE;
  error.error.function = "mult";
  error.error.scope = scope;

  if (arg2.type != VAR_TYPE)
    {
      error.error.error_code = INVALPRIM;
      return error;
    }

  mpz_mul (return_value.v_point->data, arg1.v_point->data, arg2.v_point->data);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * divide: function that divides two given     *
 * variables and returns the result. Returns   *
 * errors in the same cases as "add" and       *
 * "mult," as well as when arg2 is equal to 0. *
 * * * * * * * * * * * * * * * * * * * * * * * */

a_type divide (a_type arg1, a_type arg2)
{
  a_type return_value, error;
  func *scope;
  mpz_t zero_check; /* gmp variable to check if arg2 is zero */

  scope = NULL;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();
  
  error.type = ERROR_TYPE;
  error.error.function = "divide";
  error.error.scope = scope;

  mpz_init (zero_check);
  
  if (arg2.type != VAR_TYPE)
    {
      error.error.error_code = INVALPRIM;
      return error;
    }
  
  if (mpz_cmp (zero_check, arg2.v_point->data) == 0)
    {
      error.error.error_code = DIVNON;
      return error;
    }

  mpz_cdiv_q (return_value.v_point->data, arg1.v_point->data, arg2.v_point->data);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * mod: function that is exactly like the      *
 * "divide" function except performs modulus.  *
 * * * * * * * * * * * * * * * * * * * * * * * */

a_type mod (a_type arg1, a_type arg2)
{
  a_type return_value, error;
  func *scope;
  mpz_t zero_check;

  scope = NULL;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  error.type = ERROR_TYPE;
  error.error.function = "mod";
  error.error.scope = scope;
  
  mpz_init (zero_check);
  
  if (arg2.type != VAR_TYPE)
    {
      error.error.error_code = INVALPRIM;
      return error;
    }

  if (mpz_cmp (zero_check, arg2.v_point->data) == 0)
    {
      error.error.error_code = MODNON;
      return error;
    }
  
  mpz_mod (return_value.v_point->data, arg1.v_point->data, arg2.v_point->data);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * paren: evaluates a parenthesis. It does     *
 * this by allocating enough memory for as     *
 * many tokens that appear in between two      *
 * parens and sends it to "eval."              *
 * * * * * * * * * * * * * * * * * * * * * * * */

a_type paren (func *scope, char **words)
{
  int pos;
  int count;
  char **formatted;
  a_type error;

  error.type = ERROR_TYPE;
  error.error.function = "paren";
  error.error.scope = scope;

  pos = get_exp_length (words, ')'/*, '\0'*/);

  if (pos == 0)
    {
      error.error.error_code = SYNTAX;
      return error;
    }

  formatted = (char **) better_malloc ((sizeof (char *)) * pos);

  count = pos;

  formatted[--pos] = NULL;

  while (pos > 0)
    {
      pos--;
      formatted[pos] = words[pos];
    }

  /*
  for (count = pos, formatted[--pos] = NULL, pos--;
       pos > 0; pos--)
    formatted[pos] = words[pos];
  */

  for (pos = 0; words[count] != NULL; pos++, count++)
    words[pos] = words[count];

  words[pos] = NULL;

  return eval (scope, formatted);
}
