#include "common.h"

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

bool
isnum (char *word)
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

a_type
num_to_var (char *word)
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

a_type
add (a_type arg1, a_type arg2)
{
  a_type return_value;
  func *scope;

  scope = NULL;
  
  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();
  
  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (scope, "both arguments to + need to be vars");
  
  mpz_add (return_value.v_point->data, arg1.v_point->data, arg2.v_point->data);
  
  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * sub: function that subtracts two given      *
 * variables and returns the result. This      *
 * function when called will never return an   *
 * error.                                      *
 * * * * * * * * * * * * * * * * * * * * * * * */

a_type
sub (a_type arg1, a_type arg2)
{
  a_type return_value;
  func *scope;

  scope = NULL;

  return_value.type = VAR_TYPE;  
  return_value.v_point = alloc_var ();

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (scope, "both arguments to - need to be vars");
  
  mpz_sub (return_value.v_point->data, arg1.v_point->data, arg2.v_point->data);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * mult: function that multiplies two given    *
 * variables and returns the result. Returns   *
 * errors in the same cases as "add."          *
 * * * * * * * * * * * * * * * * * * * * * * * */

a_type
mult (a_type arg1, a_type arg2)
{
  a_type return_value;
  func *scope;

  scope = NULL;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (scope, "both arguments to * need to be vars");

  mpz_mul (return_value.v_point->data, arg1.v_point->data, arg2.v_point->data);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * divide: function that divides two given     *
 * variables and returns the result. Returns   *
 * errors in the same cases as "add" and       *
 * "mult," as well as when arg2 is equal to 0. *
 * * * * * * * * * * * * * * * * * * * * * * * */

a_type
divide (a_type arg1, a_type arg2)
{
  a_type return_value;
  func *scope;
  mpz_t zero_check; /* gmp variable to check if arg2 is zero */

  scope = NULL;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (scope, "both arguments to / need to be vars");
  
  mpz_init (zero_check);
  
  if (mpz_cmp (zero_check, arg2.v_point->data) == 0)
    return errorman_throw_reg (scope, "divide by zero error");

  mpz_tdiv_q (return_value.v_point->data, arg1.v_point->data, arg2.v_point->data);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * mod: function that is exactly like the      *
 * "divide" function except performs modulus.  *
 * * * * * * * * * * * * * * * * * * * * * * * */

a_type
mod (a_type arg1, a_type arg2)
{
  a_type return_value;
  func *scope;
  mpz_t zero_check;

  scope = NULL;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (scope, "both arguments to % need to be vars");
  
  mpz_init (zero_check);

  if (mpz_cmp (zero_check, arg2.v_point->data) == 0)
    return errorman_throw_reg (scope, "mod by zero error");
  
  mpz_mod (return_value.v_point->data, arg1.v_point->data, arg2.v_point->data);

  return return_value;
}

a_type
add_assignment (a_type arg1, a_type arg2)
{
  if (arg1.type == ERROR_TYPE)
    return arg1;
  else if (arg2.type == ERROR_TYPE)
    return arg2;

  if (arg1.type == FUNCTION_TYPE
      || arg2.type == FUNCTION_TYPE)
    return errorman_throw_reg (NULL, "both arguments to += must be variables");

  mpz_add (arg1.v_point->data, arg1.v_point->data, arg2.v_point->data);

  return arg1;
}

a_type
sub_assignment (a_type arg1, a_type arg2)
{
  if (arg1.type == ERROR_TYPE)
    return arg1;
  else if (arg2.type == ERROR_TYPE)
    return arg2;

  if (arg1.type == FUNCTION_TYPE
      || arg2.type == FUNCTION_TYPE)
    return errorman_throw_reg (NULL, "both arguments to -= must be variables");

  mpz_sub (arg1.v_point->data, arg1.v_point->data, arg2.v_point->data);

  return arg1;
}

a_type
mult_assignment (a_type arg1, a_type arg2)
{
  if (arg1.type == ERROR_TYPE)
    return arg1;
  else if (arg2.type == ERROR_TYPE)
    return arg2;

  if (arg1.type == FUNCTION_TYPE
      || arg2.type == FUNCTION_TYPE)
    return errorman_throw_reg (NULL, "both arguments to *= must be variables");

  mpz_mul (arg1.v_point->data, arg1.v_point->data, arg2.v_point->data);

  return arg1;
}

a_type
div_assignment (a_type arg1, a_type arg2)
{
  mpz_t zero_check;

  if (arg1.type == ERROR_TYPE)
    return arg1;
  else if (arg2.type == ERROR_TYPE)
    return arg2;
  
  if (arg1.type == FUNCTION_TYPE
      || arg2.type == FUNCTION_TYPE)
    return errorman_throw_reg (NULL, "both arguments to /= must be variables");

  mpz_init (zero_check);

  if (mpz_cmp (zero_check, arg2.v_point->data) == 0)
    return errorman_throw_reg (NULL, "divide by zero error");

  mpz_tdiv_q (arg1.v_point->data, arg1.v_point->data, arg2.v_point->data);

  return arg1;
}

a_type
mod_assignment (a_type arg1, a_type arg2)
{
  mpz_t zero_check;

  if (arg1.type == ERROR_TYPE)
    return arg1;
  else if (arg2.type == ERROR_TYPE)
    return arg2;
  
  if (arg1.type == FUNCTION_TYPE
      || arg2.type == FUNCTION_TYPE)
    return errorman_throw_reg (NULL, "both arguments to %= must be variables");

  mpz_init (zero_check);

  if (mpz_cmp (zero_check, arg2.v_point->data) == 0)
    return errorman_throw_reg (NULL, "mod by zero error");

  mpz_mod (arg1.v_point->data, arg1.v_point->data, arg2.v_point->data);

  return arg1;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * paren: evaluates a parenthesis. It does     *
 * this by allocating enough memory for as     *
 * many tokens that appear in between two      *
 * parens and sends it to "eval."              *
 * * * * * * * * * * * * * * * * * * * * * * * */

a_type
paren (func *scope, char **words)
{
  int pos;
  int count;
  a_type return_value;
  //char **formatted;

  pos = get_exp_length (words, ')');

  if (pos == 0)
    return errorman_throw_reg (scope, "parenthesis has no body");
  else if (words[pos - 1][0] != ')')
    return errorman_throw_reg (scope, "syntax error; expected closing ')'");

  words[pos - 1] = NULL;

  return_value = eval (scope, words);

  /*
  if (return_value.type == ERROR_TYPE)
    return return_value;
  */

  for (count = pos; words[pos] != NULL; pos++)
    words[pos - count] = words[pos];

  return return_value;
  
  
  /*

  formatted = (char **) better_malloc ((sizeof (char *)) * pos);

  count = pos;

  formatted[--pos] = NULL;

  while (pos > 0)
    {
      pos--;
      formatted[pos] = words[pos];
    }

  for (pos = 0; words[count] != NULL; pos++, count++)
    words[pos] = words[count];

  words[pos] = NULL;

  return eval (scope, formatted);
  */
}
