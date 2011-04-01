#include "casts.h"

FACT_t
var_to_string (func_t *scope)
{
  FACT_t return_value;
  char   *str;
  var_t  *op;

  op = get_var (scope, "op");
  str = mpc_get_str (op->data);  
  return_value.type = VAR_TYPE;
  
  if (strlen (str) > 1)
    {
      return_value.v_point = alloc_var ();
      return_value.v_point->array_up = string_to_array (str, NULL);
    }
  else
    return_value.v_point = string_to_array (str, NULL);

  mpz_set_si (return_value.v_point->array_size, strlen (str)); 

  return return_value;
}

FACT_t
var_to_int (func_t *scope)
{
  FACT_t return_value;

  return_value.type = VAR_TYPE;
  return_value.v_point = get_var (scope, "op");

  while (return_value.v_point->data.precision)
    {
      mpz_tdiv_q_ui (return_value.v_point->data.object,
		     return_value.v_point->data.object, 10);
      return_value.v_point->data.precision--;
    }

  return return_value;
}
