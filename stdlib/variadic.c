#include "variadic.h"

FACT_t
get_arg_type (func_t *scope)
{
  if (scope->variadic == NULL)
    FACT_ret_error (scope, "too few arguments passed to 'get_arg_type', expected one");
  else if (scope->variadic->next != NULL)
    FACT_ret_error (scope, "too many arguments passed to 'get_arg_type', expected one");
  else if (scope->variadic->type == VAR_TYPE)
    return FACT_get_ui (0);
  else
    return FACT_get_ui (1);
}

FACT_t
get_arg_num (func_t *scope)
{
  mpc_t  one;
  FACT_t return_value;
  func_t *func;
  struct FACT_mixed *go_through;
  
  /* The variable 'one' is equal to the value '1'. This is so that I don't
   * have to deal with mixing the gmp mpz interface with my custom mpc
   * interface. It is used for adding one to return_value.v_point->data.
   */
  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  /* Since the caller variable is only not NULL when the scope was called
   * as a function, we go upwards until the caller varaible is not NULL. 
   */
  
  for (func = scope->caller; func != NULL && func->caller == NULL; func = func->up); // Do nothing.

  if (func == NULL)
    return return_value;
  
  mpc_init   (one);
  mpc_set_ui (one, 1);

  for (go_through = func->variadic; go_through != NULL; go_through = go_through->next)
    mpc_add (return_value.v_point->data, return_value.v_point->data, one); 

  return return_value;
}

FACT_t
get_arg (func_t *scope)
{
  mpz_t  i;
  mpz_t  target;
  FACT_t return_value;
  var_t  *num;
  func_t *func;
  struct FACT_mixed *curr;

  num = get_var (scope, "num");

  for (func = scope->caller; func != NULL && func->caller == NULL; func = func->up); // Do nothing
  
  mpz_init (i);
  mpz_init (target);
  mpc_get_mpz (target, num->data);

  if (mpz_cmp_ui (target, 0) < 0 || func == NULL)
    FACT_ret_error (scope, "out of variadic list's bounds");

  for (curr = func->variadic; mpz_cmp (i, target) && curr != NULL; curr = curr->next)
    mpz_add_ui (i, i, 1);
  
  if (curr == NULL)
    FACT_ret_error (scope, "out of variadic list's bounds");

  if ((return_value.type = curr->type) == VAR_TYPE)
    return_value.v_point = curr->var_p;
  else
    return_value.f_point = curr->func_p;

  return return_value;
}
