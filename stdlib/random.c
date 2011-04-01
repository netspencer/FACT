#include "random.h"

FACT_t
simple_rand (func_t * scope)
{
  // simple_rand - generate a simple random number.
  FACT_t return_value;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  srand (time (NULL));
  mpc_set_si (return_value.v_point->data, rand ());

  return return_value;
}
