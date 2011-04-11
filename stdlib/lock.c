#include "lock.h"

static void
lock_var_array (var_t *array)
{
  while (array != NULL)
    {
      lock_var_array (array->array_up);
      array->locked = true;
      array = array->next;
    }
}

static void
lock_func_array (func_t *array)
{
  while (array != NULL)
    {
      lock_func_array (array->array_up);
      array->locked = true;
      array = array->next;
    }
}

FACT_t
lock_object (func_t *scope)
{
  FACT_t return_value;
  var_t  *obj_name;
  var_t  *obj_var;
  func_t *obj_func;
  func_t *search;

  search = get_func (scope, "search");
  obj_name = get_var (scope, "name");
  obj_var = get_var (search, array_to_string (obj_name));

  if (obj_var == NULL)
    {
      obj_func = get_func (search, array_to_string (obj_name));
      if (obj_func == NULL)
	FACT_ret_error (scope->up, "no such object exists");
    }

  if (obj_var != NULL)
    {
      return_value.type = VAR_TYPE;
      obj_var->locked = true;
      return_value.v_point = obj_var;
      lock_var_array (obj_var->array_up);
    }
  else
    {
      return_value.type = FUNCTION_TYPE;
      obj_func->locked = true;
      return_value.f_point = obj_func;
      lock_func_array (obj_func->array_up);
    }

  return return_value;
}
