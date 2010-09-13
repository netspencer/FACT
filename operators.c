#include "interpreter.h"

/*---------------------------------------------*
 * operators.c: Provides functions for         *
 * comparring data and logical operators.      *
 *                                             *
 * This code is (C) 2010 Matthew Plant.        *
 *---------------------------------------------*/

/*---------------------------------------------*
 * Functions: The actual code. All of these    *
 * functions will return 1 if the arguments    *
 * passed to it meet their conditions, and 0   *
 * if they do not.                             *
 *---------------------------------------------*/

/* * * * * * * * * * * * * * * * * * * * * * * *
 * equal: checks if two arguments are equal to *
 * each other.                                 *
 * * * * * * * * * * * * * * * * * * * * * * * */

a_type equal (a_type arg1, a_type arg2)
{
  a_type return_value, error;
  func *scope;

  scope = NULL;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();
  
  error.type = ERROR_TYPE;
  error.error.function = "equal";
  error.error.scope = scope;

  if (arg1.type == ERROR_TYPE || arg2.type == ERROR_TYPE)
    {
      error.error.error_code = LESSPRIM; /* if there is a missing argument */
      return error;
    }

  if (arg1.type != arg2.type)
    {
      error.error.error_code = INVALPRIM; /* if the arguments' types do not match */
      return error;
    }
  
  if (arg1.type == VAR_TYPE)
    {
      if (mpz_cmp (arg1.v_point->data, arg2.v_point->data) == 0)
        mpz_set_si (return_value.v_point->data, 1);
      else
        mpz_set_si (return_value.v_point->data, 0);
    }
  else /* this block does not work, will be fixed when I give a damn */ 
    {
      if (arg1.f_point == arg2.f_point)
        mpz_set_si (return_value.v_point->data, 1);
      else
        mpz_set_si (return_value.v_point->data, 0);
    }

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * *
 * not_equal: checks if two arguments are    *
 * not equal to each other.                  *
 * * * * * * * * * * * * * * * * * * * * * * */

a_type not_equal (a_type arg1, a_type arg2)
{
  a_type return_value, error;
  func *scope;

  scope = NULL;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  error.type = ERROR_TYPE;
  error.error.function = "not_equal";
  error.error.scope = scope;

  if (arg1.type == ERROR_TYPE || arg2.type == ERROR_TYPE)
    {
      error.error.error_code = LESSPRIM; /* if there is a missing argument */
      return error;
    }

  if (arg1.type != arg2.type)
    {
      error.error.error_code = INVALPRIM; /* if the arguments' types do not match */
      return error;
    }
    
  if (arg1.type == VAR_TYPE)
    {
      if (mpz_cmp (arg1.v_point->data, arg2.v_point->data) == 0)
        mpz_set_si (return_value.v_point->data, 0);
      else
        mpz_set_si (return_value.v_point->data, 1);
    }
  else /* this block does not work, will be fixed when I give a damn */ 
    {
      if (arg1.f_point == arg2.f_point)
        mpz_set_si (return_value.v_point->data, 0);
      else
        mpz_set_si (return_value.v_point->data, 1);
    }

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * *
 * more: checks if the first argument is     *
 * greater in value than the other arguemnt. *
 * If either argument is not a variable, it  *
 * throws an error.                          *
 * * * * * * * * * * * * * * * * * * * * * * */

a_type more (a_type arg1, a_type arg2)
{
  a_type return_value, error;
  func *scope;

  scope = NULL;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  error.type = ERROR_TYPE;
  error.error.function = "more";
  error.error.scope = scope;

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    {
      error.error.error_code = INVALPRIM; /* if either of the arguments are invalid */
      return error;
    }

  if (mpz_cmp (arg1.v_point->data, arg2.v_point->data) > 0)
      mpz_init_set_si (return_value.v_point->data, 1);
  else
      mpz_init_set_si (return_value.v_point->data, 0);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * *
 * more_equal: checks if the first argument  *
 * is greater or equal in value to the other *
 * arguemnt. If either argument is not a     *
 * variable, it throws an error.             *
 * * * * * * * * * * * * * * * * * * * * * * */

a_type more_equal (a_type arg1, a_type arg2)
{
  a_type return_value, error;
  func *scope;

  scope = NULL;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  error.type = ERROR_TYPE;
  error.error.function = "more_equal";
  error.error.scope = scope;

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    {
      error.error.error_code = INVALPRIM; /* if either of the arguments are invalid */
      return error;
    }
  
  if (mpz_cmp (arg1.v_point->data, arg2.v_point->data) >= 0)
      mpz_init_set_si (return_value.v_point->data, 1);
  else
      mpz_init_set_si (return_value.v_point->data, 0);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * *
 * less: checks if the first argument is     *
 * smaller in value than the other arguemnt. *
 * If either argument is not a variable, it  *
 * throws an error.                          *
 * * * * * * * * * * * * * * * * * * * * * * */

a_type less (a_type arg1, a_type arg2)
{
  a_type return_value, error;
  func *scope;

  scope = NULL;
  
  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  error.type = ERROR_TYPE;
  error.error.function = "less";
  error.error.scope = scope;

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    {
      error.error.error_code = INVALPRIM; /* if either of the arguments are invalid */
      return error;
    }

  if (mpz_cmp (arg1.v_point->data, arg2.v_point->data) < 0)
      mpz_init_set_si (return_value.v_point->data, 1);
  else
      mpz_init_set_si (return_value.v_point->data, 0);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * *
 * less_equal: checks if the first argument  *
 * is smaller or equal in value to the other *
 * arguemnt. If either argument is not a     *
 * variable, it throws an error.             *
 * * * * * * * * * * * * * * * * * * * * * * */

a_type less_equal (a_type arg1, a_type arg2)
{
  a_type return_value, error;
  func *scope;

  scope = NULL;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  error.type = ERROR_TYPE;
  error.error.function = "less_equal";
  error.error.scope = scope;

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    {
      error.error.error_code = INVALPRIM; /* if either of the arguments are invalid */
      return error;
    }

  if (mpz_cmp (arg1.v_point->data, arg2.v_point->data) <= 0)
      mpz_init_set_si (return_value.v_point->data, 1);
  else
      mpz_init_set_si (return_value.v_point->data, 0);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * *
 * and: checks if both arguments are true.   *
 * In order for an argument to be true it    *
 * must not be equal to zero.                *
 * * * * * * * * * * * * * * * * * * * * * * */

a_type and (a_type arg1, a_type arg2)
{
  a_type return_value, error;
  func *scope;

  scope = NULL;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  error.type = ERROR_TYPE;
  error.error.function = "and";
  error.error.scope = scope;

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    {
      error.error.error_code = INVALPRIM; /* if either of the arguments are invalid */
      return error;
    }
  
  mpz_init_set_si (return_value.v_point->data, 0); /* set the return value to false as the default */ 

  if (mpz_cmp (arg1.v_point->data, return_value.v_point->data) != 0
      && mpz_cmp (arg2.v_point->data, return_value.v_point->data) != 0) /*
									  only change the return value if both of the arguments
									  evaluate to true.
									*/
    mpz_set_si (return_value.v_point->data, 1);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * *
 * or: checks if at least one of the       *
 * passed arguments evaluate to true.      *
 * * * * * * * * * * * * * * * * * * * * * */

a_type or (a_type arg1, a_type arg2)
{
  a_type return_value, error;
  func *scope;

  scope = NULL;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  error.type = ERROR_TYPE;
  error.error.function = "or";
  error.error.scope = scope;

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    {
      error.error.error_code = INVALPRIM; /* if either of the arguments are invalid */
      return error;
    }

  mpz_init_set_si (return_value.v_point->data, 0);

  if (mpz_cmp (arg1.v_point->data, return_value.v_point->data) != 0
      || mpz_cmp (arg2.v_point->data, return_value.v_point->data) != 0) /*
									  same method as the one in function "and", but turning
									  return_value to true if at least one of the arguments
									  evaluate to true.
									*/
    mpz_set_si (return_value.v_point->data, 1);

  return return_value;
}
