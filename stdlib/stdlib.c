#include <FACT/FACT.h>

FACT_t
ft_putchar (func_t *scope)
{
  var_t *to_put;

  to_put = get_var (scope, "char");
  putchar (var_get_si (to_put));

  return FACT_get_ui (0);
}

struct elements MOD_MAP [] =
  {
    { "putchar", "def char ", &ft_putchar },
    { NULL     , NULL       , NULL        }
  };
