#include "errorman.h"

FACT_t
errorman_throw_reg (func_t *scope, char *description)
{
  FACT_t return_value;

  return_value.type              = ERROR_TYPE;
  return_value.error.scope       = scope;
  return_value.error.description = description;
  return_value.error.thrown      = false;

  return return_value;
}

FACT_t
errorman_throw_catchable (func_t *scope, char *description)
{
  FACT_t return_value;

  return_value.type              = ERROR_TYPE;
  return_value.error.scope       = scope;
  return_value.error.description = description;
  return_value.error.thrown      = true;

  return return_value;
}

void
errorman_dump (_ERROR error, int line_num, const char *file_name_called, const char *file_name_from)
{
  if (error.thrown)
    {
      printf ("Caught error from function [%s]: %s.\n", error.scope->name, error.description);
      return;
    }
  
  printf ("Error in <%s:%s> on line %d", file_name_called, file_name_from, line_num);

  printf (": %s.\n",  error.description);
}
