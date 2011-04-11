/* This file is part of FACT.
 *
 * FACT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FACT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FACT. If not, see <http://www.gnu.org/licenses/>.
 *
 * ---------------------------------------------------------------------
 *
 * BIFs.c - a collections of misc. built-in-functions for FACT. These
 * functions don't really fit anywhere else, and thus they are placed
 * here.
 */

#include "FACT.h"

FACT_DEFINE_BIF (sizeof, "->")
{
  /**
   * sizeof - determines the array size of a passed variable.
   * It takes one and only one argument that can either be a
   * variable or a function.
   */
  FACT_t return_value;

  if (scope->variadic == NULL || scope->variadic->next != NULL)
    FACT_ret_error (scope, "sizeof expects one argument");

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();
  
  if (scope->variadic->type == VAR_TYPE)
    mpz_set (return_value.v_point->data.object,
	     scope->variadic->var_p->array_size);
  else
    mpz_set (return_value.v_point->data.object,
	     scope->variadic->func_p->array_size);

  return return_value;
}

FACT_DEFINE_BIF (ref, "->")
{
  /**
   * ref - returns the address of the variable passed. This
   * is equivalent to the & C operator. If the argument 
   * passed is a function, then the returned value will be
   * negative to denote the type.
   */
  FACT_t return_value;

  if (scope->variadic == NULL || scope->variadic->next != NULL)
    FACT_ret_error (scope, "ref expects one argument");

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (scope->variadic->type == VAR_TYPE)
    mpz_set_ui (return_value.v_point->data.object,
                (unsigned long) scope->variadic->var_p);
  else
    {
      /* Because the pointer size is an unsigned long, we don't
       * want to reduce the conversion to one step with the use
       * of mpz_set_si, as loss of data may occur.
       */
      mpz_set_ui (return_value.v_point->data.object,
                  (unsigned long) scope->variadic->func_p);
      mpz_neg (return_value.v_point->data.object, return_value.v_point->data.object);
    }

  return return_value;
}

#ifdef SAFE
// Some simple functions for checking arrays.
static bool
check_func_array (func_t *curr, func_t *op)
{
  for (; curr != NULL; curr = curr->next)
    {
      if (curr == op || check_func_array (curr->array_up, op))
        return true;
    }
  return false;
}

static bool
check_var_array (var_t *curr, var_t *op)
{
  for (; curr != NULL; curr = curr->next)
    {
      if (curr == op || check_var_array (curr->array_up, op))
        return true;
    }
  return false;
}
#endif /* SAFE */

FACT_DEFINE_BIF (deref, "def op")
{
  /**
   * deref - returns the variable or function located at
   * the address passed. If the macro SAFE is defined, we
   * also check to make sure that the address is valid.
   * This function is equivalent to the * C operator.
   *
   * @op: Value that contains the address of the 
   *      variable/function to dereference. 
   */
  var_t *op;
#ifdef SAFE
  func_t *curr;
#endif
  FACT_t ret_val;
  unsigned long addr;
  
  op = get_var (scope, "op");
  
  // Check to make sure that op is not floating point.
  if (op->data.precision > 1)
    FACT_ret_error (scope, "addresses cannot be floating point");

  addr = mpz_get_ui (op->data.object);
  ret_val.type = ((mpz_sgn (op->data.object) < 0)
                  ? FUNCTION_TYPE
                  : VAR_TYPE);
  
#ifdef SAFE
  /* There will be a routine here to check the validity of
   * the address. It's very slow, so we don't make it
   * needed to build.
   */
  for (curr = scope->caller; curr != NULL; curr = curr->up)
    {
      if ((ret_val.type == FUNCTION_TYPE
           && check_func_array (curr->funcs, (func_t *) addr))
          || check_var_array (curr->vars, (var_t *) addr))
        goto FOUND;
    }
  /* We've looped through every valid scope and it was no
   * where to be found.
   */
  FACT_ret_error (scope, "invalid address");

 FOUND:
#endif /* SAFE */

  /* We just set both of the pointers to the address, as the
   * invalid one will just be ignored.
   */
  ret_val.v_point = (var_t *) addr;
  ret_val.f_point = (func_t *) addr;

  return ret_val;
}

FACT_DEFINE_BIF (print, "def str")
{
  /**
   * print - print out a single string with no additives (such as a
   * newline or what not). This should be used when printing messages
   * from seperate threads.
   *
   * @str: The string to print.
   */
  var_t *str;

  str = get_var (scope, "str");
  printf ("%s", array_to_string (str)); // Seperation used to remove warnings.

  return FACT_get_ui (0); // Always returns 0.
}

FACT_DEFINE_BIF (str, "def val")
{
  /**
   * str - converts a variable into a string array. If passed an array, 
   * it will only convert the base value.
   *
   * @val: value to convert.
   */
  char *str;
  FACT_t return_str;

  str = mpc_get_str (get_var (scope, "val")->data);

  return_str.return_signal = false;
  return_str.break_signal  = false;
  return_str.type          = VAR_TYPE;

  if (strlen (str) > 1)
    {
      return_str.v_point = alloc_var ();
      return_str.v_point->array_up = string_to_array (str, NULL);
    }
  else
    return_str.v_point = string_to_array (str, NULL);

  mpz_set_si (return_str.v_point->array_size, strlen (str));

  return return_str;
}

///////////////////////
// Locking functions:
///////////////////////

static void
lock_var_array (var_t *curr)
{
  for (; curr != NULL; curr = curr->next)
    {
      lock_var_array (curr->array_up);
      curr->locked = true;
    }
}

static void
lock_func_array (func_t *curr)
{
  for (; curr != NULL; curr = curr->next)
    {
      lock_func_array (curr->array_up);
      curr->locked = true;
    }
}

FACT_DEFINE_BIF (lock, "->")
{
  /**
   * lock - lock the passed variables/functions and prevent
   * changes in their values. Note, this does not prevent elements
   * of a scope being changed, just function pointer itself.
   */
  struct FACT_mixed *curr;
  
  if (scope->variadic == NULL)
    FACT_ret_error (scope, "lock expects at least one argument");

  for (curr = scope->variadic; scope != NULL; scope = scope->next)
    {
      if (curr->type == FUNCTION_TYPE)
        lock_func_array (curr->func_p);
      else 
        lock_var_array (curr->var_p);
    }

  return FACT_get_ui (0);
}
