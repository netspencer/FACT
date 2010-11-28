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
  bool         hex;
  bool         flp;
  unsigned int pos;

  if (word[0] == '0' && tolower (word[1]) == 'x')
    hex = true;
  else
    hex = false;
  
  for (pos = (hex) ? 2 : 0, flp = false; word[pos] != '\0'; pos++)
    {
      if (word[pos] == '.')
	{
	  if (flp || word[pos + 1] == '\0')
	    return false;
	  flp = true;
	}
      else if (!isdigit ((int) word[pos]))
	if (!hex || tolower (word[pos]) < 'a' || tolower (word[pos]) > 'f')
	  return false;
    }

  return true;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * get_prec: This function takes a correctly   *
 * formatted string, counts the number of      *
 * digits after the decimal point, and         *
 * replaces the decimal point with a '.'       *
 * * * * * * * * * * * * * * * * * * * * * * * */

unsigned int
get_prec (char *word)
{
  bool         is_decimal;
  unsigned int pos;
  unsigned int precision;

  for (pos = precision = 0, is_decimal = false; word[pos] != '\0'; pos++)
    {
      if (is_decimal)
	precision++;

      if (word[pos] == '.')
	{
	  is_decimal = true;
	  word[pos] = ' ';
	}
    }

  return precision;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * num_to_var: really simple function that     *
 * takes in a string and converts it to a      *
 * var_tiable type wrapped in an ambigious type. *
 * * * * * * * * * * * * * * * * * * * * * * * */

FACT_t
num_to_var (char *word)
{
  FACT_t return_value;
  
  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (word[0] == '0' && tolower (word[1]) == 'x')
    mpc_set_str (&(return_value.v_point->data), word + 2, get_prec (word), 16);
  else
    mpc_set_str (&(return_value.v_point->data), word, get_prec (word), 10);

  return_value.v_point->array_size = 1;
  return_value.v_point->array_up   = NULL;
  return_value.v_point->next       = NULL;
  
  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * add: function that adds two given           *
 * var_tiables and returns the result, or an     *
 * error if the second var_tiable is missing.    *
 * * * * * * * * * * * * * * * * * * * * * * * */

FACT_t
add (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;
  
  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();
  
  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (NULL, "both arguments to + need to be vars");
  
  mpc_add (&(return_value.v_point->data), arg1.v_point->data, arg2.v_point->data);
  
  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * sub: function that subtracts two given      *
 * var_tiables and returns the result. This      *
 * function when called will never return an   *
 * error.                                      *
 * * * * * * * * * * * * * * * * * * * * * * * */

FACT_t
sub (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;

  return_value.type    = VAR_TYPE;  
  return_value.v_point = alloc_var ();

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (NULL, "both arguments to - need to be vars");
  
  mpc_sub (&(return_value.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * mult: function that multiplies two given    *
 * var_tiables and returns the result. Returns   *
 * errors in the same cases as "add."          *
 * * * * * * * * * * * * * * * * * * * * * * * */

FACT_t
mult (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (NULL, "both arguments to * need to be vars");

  mpc_mul (&(return_value.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * divide: function that divides two given     *
 * var_tiables and returns the result. Returns   *
 * errors in the same cases as "add" and       *
 * "mult," as well as when arg2 is equal to 0. *
 * * * * * * * * * * * * * * * * * * * * * * * */

FACT_t
divide (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (NULL, "both arguments to / need to be vars");
  
  if (mpc_cmp_si (arg2.v_point->data, 0) == 0)
    return errorman_throw_reg (NULL, "divide by zero error");

  mpc_div (&(return_value.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * mod: function that is exactly like the      *
 * "divide" function except performs modulus.  *
 * * * * * * * * * * * * * * * * * * * * * * * */

FACT_t
mod (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (NULL, "both arguments to % need to be vars");

  if (mpc_cmp_si (arg2.v_point->data, 0) == 0)
    return errorman_throw_reg (NULL, "mod by zero error");
  
  mpc_mod (&(return_value.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return return_value;
}

FACT_t
bit_and (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (NULL, "both arguments to & need to be vars");
  
  mpc_and (&(return_value.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return return_value;
}

FACT_t
bit_ior (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (NULL, "both arguments to & need to be vars");
  
  mpc_ior (&(return_value.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return return_value;
}

FACT_t
bit_xor (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (NULL, "both arguments to & need to be vars");
  
  mpc_xor (&(return_value.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return return_value;
}

FACT_t
add_assignment (FACT_t arg1, FACT_t arg2)
{
  if (arg1.type == ERROR_TYPE)
    return arg1;
  else if (arg2.type == ERROR_TYPE)
    return arg2;

  if (arg1.type == FUNCTION_TYPE
      || arg2.type == FUNCTION_TYPE)
    return errorman_throw_reg (NULL, "both arguments to += must be variables");

  if (arg1.v_point->locked)
    return add (arg1, arg2);

  mpc_add (&(arg1.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return arg1;
}

FACT_t
sub_assignment (FACT_t arg1, FACT_t arg2)
{
  if (arg1.type == ERROR_TYPE)
    return arg1;
  else if (arg2.type == ERROR_TYPE)
    return arg2;

  if (arg1.type == FUNCTION_TYPE
      || arg2.type == FUNCTION_TYPE)
    return errorman_throw_reg (NULL, "both arguments to -= must be variables");

  if (arg1.v_point->locked)
    return sub (arg1, arg2);
  
  mpc_sub (&(arg1.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return arg1;
}

FACT_t
mult_assignment (FACT_t arg1, FACT_t arg2)
{
  if (arg1.type == ERROR_TYPE)
    return arg1;
  else if (arg2.type == ERROR_TYPE)
    return arg2;

  if (arg1.type == FUNCTION_TYPE
      || arg2.type == FUNCTION_TYPE)
    return errorman_throw_reg (NULL, "both arguments to *= must be variables");

  if (arg1.v_point->locked)
    return mult (arg1, arg2);

  mpc_mul (&(arg1.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return arg1;
}

FACT_t
div_assignment (FACT_t arg1, FACT_t arg2)
{
  if (arg1.type == ERROR_TYPE)
    return arg1;
  else if (arg2.type == ERROR_TYPE)
    return arg2;
  
  if (arg1.type == FUNCTION_TYPE
      || arg2.type == FUNCTION_TYPE)
    return errorman_throw_reg (NULL, "both arguments to /= must be variables");

  if (mpc_cmp_si (arg2.v_point->data, 0) == 0)
    return errorman_throw_reg (NULL, "divide by zero error");

  if (arg1.v_point->locked)
    return divide (arg1, arg2);

  mpc_div (&(arg1.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return arg1;
}

FACT_t
mod_assignment (FACT_t arg1, FACT_t arg2)
{
  if (arg1.type == ERROR_TYPE)
    return arg1;
  else if (arg2.type == ERROR_TYPE)
    return arg2;
  
  if (arg1.type == FUNCTION_TYPE
      || arg2.type == FUNCTION_TYPE)
    return errorman_throw_reg (NULL, "both arguments to %= must be variables");

  if (mpc_cmp_si (arg2.v_point->data, 0) == 0)
    return errorman_throw_reg (NULL, "mod by zero error");

  if (arg1.v_point->locked)
    return mod (arg1, arg2);

  mpc_mod (&(arg1.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return arg1;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
 * paren: evaluates a parenthesis. It does     *
 * this by allocating enough memory for as     *
 * many tokens that appear in between two      *
 * parens and sends it to "eval."              *
 * * * * * * * * * * * * * * * * * * * * * * * */

FACT_t
paren (func_t *scope, word_list expression)
{
  int    pos;
  FACT_t return_value;
  
  pos = get_exp_length (expression.syntax, ')');

  if (pos == 0)
    return errorman_throw_reg (scope, "parenthesis has no body");
  else if (expression.syntax[pos - 1][0] != ')')
    return errorman_throw_reg (scope, "syntax error; expected closing ')'");

  expression.syntax[pos - 1]       = NULL;
  return_value                     = eval (scope, expression);
  expression.syntax[pos - 1]       = ")";
  expression.move_forward[pos - 1] = true;

  return return_value;
}
