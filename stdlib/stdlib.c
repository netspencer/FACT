#include "stdlib.h"

FACT_t
ft_putchar (func_t *scope)
{
  var_t *to_put;

  to_put = get_var (scope, "char");
  putchar (var_get_si (to_put));

  return FACT_get_ui (0);
}

FACT_t
lock_object (func_t *scope)
{
  var_t  * obj_name;
  var_t  * obj_var;
  func_t * obj_func;
  func_t * search;
  FACT_t   return_value;

  search = get_func (scope, "search");

  obj_name = get_var (scope, "name");
  obj_var = get_var (search, array_to_string (obj_name));

  if (obj_var == NULL)
    {
      obj_func = get_func (search, array_to_string (obj_name));
      if (obj_func == NULL)
	return errorman_throw_reg (scope->up, "no object of that name exists");
    }

  if (obj_var != NULL)
    {
      return_value.type = VAR_TYPE;
      obj_var->locked = true;
      return_value.v_point = obj_var;
    }
  else
    {
      return_value.type = FUNCTION_TYPE;
      obj_func->locked = true;
      return_value.f_point = obj_func;
    }

  return return_value;
}
  
    
FACT_t
throw_error (func_t *scope)
{
  char  * description;
  var_t * to_throw;
  FACT_t  return_value;

  to_throw = get_var (scope, "description");

  description = array_to_string (to_throw);

  return_value.type = ERROR_TYPE;
  return_value.error.scope = scope->up;
  return_value.error.description = description;
  return_value.error.thrown = true;

  return return_value;
}

struct elements MOD_MAP [] =
  {
    { "run"     , "def filename"           , &run_file_soft },
    { "run_loud", "def filename"           , &run_file_loud },
    { "putchar" , "def char"               , &ft_putchar    },
    { "lock"    , "defunc search, def name", &lock_object   },
    { "throw"   , "def description"        , &throw_error   },
    { NULL      , NULL                     , NULL           }
  };
