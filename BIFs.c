/* BIFs.c - a collections of misc. built-in-functions for FACT. There
 * are BIFs that are defined in other files, however they fit the
 * context of the entire file. These don't really fit anywhere else.
 */

#include "FACT.h"

FACT_DEFINE_BIF (sizeof, "->")
{
  /* sizeof - determines the array size of a passed variable.
   * It takes one and only one argument that can either be a
   * variable or a function.
   */
  FACT_t return_value;

  if (scope->variadic == NULL || scope->variadic->next != NULL)
    errorman_throw_catchable (scope, "sizeof expects one argument");

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
