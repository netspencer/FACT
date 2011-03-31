/* BIFs.c - a collections of misc. built-in-functions for FACT. There
 * are BIFs that are defined in other files, however they fit the
 * context of the entire file. These don't really fit anywhere else.
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
  var_t         *op;
  FACT_t return_value;
  unsigned long address; 

  op = get_var (scope, "op");
  
  // Check to make sure that op is not floating point.
  if (op->data.precision > 1)
    FACT_ret_error (scope, "addresses cannot be floating point");

  address = mpz_get_ui (op->data.object);
  return_value.type = (mpz_sgn (op->data.object) < 0)
    ? FUNCTION_TYPE
    : VAR_TYPE;
  
#ifdef SAFE
  /* There will be a routine here to check the validity of
   * the address. Right now that has been left out, but I
   * plan to add it very soon.
   */
#endif /* SAFE */

  // We just set both of the pointers to the address, as the
  // invalid one will just be ignored. 
  return_value.v_point = (var_t *) address;
  return_value.f_point = (func_t *) address;

  return return_value;
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
   * str - converts a variable into a string array. If passed an array, it
   * will only convert the base value.
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
