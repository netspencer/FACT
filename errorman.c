#include "errorman.h"

a_type
errorman_throw_reg (func *scope, char *description)
{
  a_type return_value;

  return_value.type = ERROR_TYPE;

  return_value.error.scope = scope;
  return_value.error.description = description;

  return_value.error.thrown = false;

  return return_value;
}

a_type
errorman_throw_prim (func *scope, char **words)
{
  a_type return_value;
  a_type evald;
  char *description;

  evald = eval (scope, words);

  if (evald.type == ERROR_TYPE)
    return evald;
  if (evald.type == FUNCTION_TYPE)
    return errorman_throw_reg (scope, "cannot throw a function as an error");

  description = array_to_string (evald.v_point);

  return_value.type = ERROR_TYPE;

  return_value.error.scope = scope;
  return_value.error.description = description;

  return_value.error.thrown = true;

  return return_value;
}

void
errorman_dump (_ERROR error, int line_num, const char *filename)
{
  if (error.thrown)
    {
      printf ("Caught error from function [%s]: %s.\n", error.scope->name, error.description);
      return;
    }
  
  printf ("Error in <%s> on line %d", filename, line_num);

  if (error.scope != NULL)
    printf (", function [%s]", error.scope->name);

  printf (": %s.\n",  error.description);
}
