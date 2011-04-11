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
 */

#include "FACT.h"

FACT_DEFINE_BIF (type, "->")
{
  if (scope->variadic == NULL || scope->variadic->next != NULL)
    FACT_ret_error (scope, "expected one argument");

  return FACT_get_ui ((scope->variadic->type == VAR_TYPE)
                      ? 0
                      : 1);
}

FACT_DEFINE_BIF (args, NOARGS)
{
  /**
   * args - get the number of variadic arguments in a
   * given scope.
   */
  FACT_t ret_val;
  func_t *curr_func;
  struct FACT_mixed *curr_mix;

  ret_val.type = VAR_TYPE;
  ret_val.v_point = alloc_var ();

  /* Go to the calling function, moving through any scope that was
   * not created through a function call.
   */
  for (curr_func = scope->caller; curr_func != NULL && curr_func->caller == NULL; curr_func = curr_func->up); // Do nothing.

  if (curr_func == NULL)
    return ret_val;

  // Count the number of arguments.
  for (curr_mix = curr_func->variadic; curr_mix != NULL; curr_mix = curr_mix->next)
    mpc_add_ui (&(ret_val.v_point->data), ret_val.v_point->data, 1);

  return ret_val;
}

FACT_DEFINE_BIF (arg, "def op")
{
  mpz_t i;
  FACT_t ret_val;
  var_t  *op;
  func_t *curr_func;
  struct FACT_mixed *curr_mix;

  op = get_var (scope, "op");

  if (mpz_cmp_ui (op->data.object, 0) < 0 || op->data.precision != 0)
    FACT_ret_error (scope, "out of variaic list's bounds");

  for (curr_func = scope->caller; curr_func != NULL && curr_func->caller == NULL; curr_func = curr_func->up); // Do nothing.
  
  if (curr_func == NULL)
    FACT_ret_error (scope, "out of variadic list's bounds");

  mpz_init (i);
  for (curr_mix = curr_func->variadic; mpz_cmp (i, op->data.object) && curr_mix != NULL; curr_mix = curr_mix->next)
    mpz_add_ui (i, i, 1);

  if (curr_mix == NULL)
    FACT_ret_error (scope, "out of variadic list's bounds");

  ret_val.type = curr_mix->type;
  if (ret_val.type == VAR_TYPE)
    ret_val.v_point = clone_var_f (curr_mix->var_p, NULL);
  else
    ret_val.f_point = curr_mix->func_p;

  return ret_val;
}
