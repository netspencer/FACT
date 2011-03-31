#include "FACT.h"

FACT_t
FACT_throw_error (func_t *scope, const char *desc, syn_tree_t exp)
{
  int i;
  FACT_t ret_val;

  ret_val.type = ERROR_TYPE;
  ret_val.error.line = scope->line;
  ret_val.error.description = (char *) desc;
  ret_val.error.scope = scope;

  if (exp.lines != NULL)
    {
      for (i = 0; i < (*exp.syntax - exp.base); i++)
        ret_val.error.line += exp.lines[i];
    }
  
  return ret_val;
}

void
errorman_dump (error_t error)
{
  fprintf (stderr, "E> Caught error in %s at line %d: %s.\n", error.scope->name, error.line, error.description);
}
