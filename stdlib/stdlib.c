#include <FACT/FACT.h>

FACT_t
ft_putchar (func_t *scope)
{
  var_t *to_put;

  to_put = get_var (scope, "char");
  putchar (var_get_si (to_put));

  return FACT_get_ui (0);
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
    { "putchar", "def char"        , &ft_putchar  },
    { "throw"  , "def description" , &throw_error },
    { NULL     , NULL              , NULL         }
  };
