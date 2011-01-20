#include "errorman.h"

FACT_t
errorman_throw_reg (func_t * scope, char * description)
{
  FACT_t return_value;

  return_value.type              = ERROR_TYPE  ;
  return_value.error.scope       = scope       ;
  return_value.error.description = description ;
  return_value.error.thrown      = false       ;

  return return_value;
}

FACT_t
errorman_throw_catchable (func_t * scope, char * description)
{
  FACT_t return_value;

  return_value.type              = ERROR_TYPE  ;
  return_value.error.scope       = scope       ;
  return_value.error.description = description ;
  return_value.error.thrown      = true        ;

  return return_value;
}

void
errorman_dump (_ERROR error)
{
  if (error.thrown)
    printf ("Caught error from function [%s]: %s.\n", error.scope->name, error.description);
  else
    {
      printf ("Error in <%s> on line %d", error.scope->file_name, error.scope->line);
      printf (": %s.\n",  error.description);
    }
}
