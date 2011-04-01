#include "FACT.h"

FACT_t
FACT_throw_error (func_t *scope, const char *desc, syn_tree_t exp)
{
  char **i;
  FACT_t ret_val =
    {
      .type = ERROR_TYPE,
      .error =
      {
        .line = scope->line,
        .description = (char *) desc,
        .scope = scope,
      },
    };

  if (exp.lines != NULL)
    {
      for (i = exp.base; i <= exp.syntax; i++)
        ret_val.error.line += *(exp.lines++);
    }
  
  return ret_val;
}

void
errorman_dump (error_t error)
{
  fprintf (stderr, "E> Caught error \"%s\" from %s at (%s:%d).\n", error.description, error.scope->name, error.scope->file_name, error.line);
}
