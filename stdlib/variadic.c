#include "variadic.h"

FACT_t
get_arg_type (func_t *scope)
{
  /* This function returns the type
   * of the variable passed to it.
   * If more or less than one
   * variable is passed, an error
   * is thrown.
   *
   * This returns 0 if it's a variable,
   * and 1 if it's a function.
   */
  
  if (scope->variadic == NULL)
    return errorman_throw_catchable (scope, "too few arguments passed to 'get_arg_type', expected one");
  else if (scope->variadic->next != NULL)
    return errorman_throw_catchable (scope, "too many arguments passed to 'get_arg_type', expected one");
  else if (scope->variadic->type == VAR_TYPE)
    return FACT_get_ui (0);
  else
    return FACT_get_ui (1);
}

FACT_t
get_arg_num (func_t *scope)
{
  /* Get the number of variadic arguments
   * in a given function (the caller).
   */
  mpc_t           one;
  func_t        * func;
  FACT_t          return_value;
  struct _MIXED * go_through;
  
  /* The variable 'one' is equal to the value '1'.
   * This is so that I don't have to deal with
   * mixing the gmp mpz interface with my custom
   * mpc interface. It is used for adding one to
   * the variable 'return_value.v_point->data'.
   */

  return_value.return_signal = false;
  return_value.break_signal  = false;
  return_value.type          = VAR_TYPE;
  return_value.v_point       = alloc_var ();

  /* Since the caller variable is only not 
   * NULL when the scope was called as a
   * function, we go upwards until the
   * caller varaible is not NULL. 
   */
  
  for (func = scope->caller; func != NULL && func->caller == NULL; func = func->up)
    ;

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
  mpz_t           index;
  mpz_t           target;
  var_t         * num;
  func_t        * func;
  FACT_t          return_value;
  struct _MIXED * go_through;

  /* This function gets the num'th variadic
   * argument of function inside, starting at
   * zero.
   */

  num = get_var (scope, "num");

  /* This does the same thing as get_arg_num,
   * look at the previous function for a
   * detailed comment.
   */
  for (func = scope->caller; func != NULL && func->caller == NULL; func = func->up)
    ;
  
  mpz_init    (index);
  mpz_init    (target);
  mpc_get_mpz (target, num->data);

  if (mpz_cmp_ui (target, 0) < 0 || func == NULL)
    return errorman_throw_catchable (scope, "out of variadic list's bounds");

  for (go_through = func->variadic; mpz_cmp (index, target) && go_through != NULL; go_through = go_through->next)
    mpz_add_ui (index, index, 1);
  
  if (go_through == NULL)
    return errorman_throw_catchable (scope, "out of variadic list's bounds");

  if ((return_value.type = go_through->type) == VAR_TYPE)
    return_value.v_point = go_through->var_p;
  else
    return_value.f_point = go_through->func_p;

  return_value.return_signal = false;
  return_value.break_signal  = false;
  
  return return_value;
}
