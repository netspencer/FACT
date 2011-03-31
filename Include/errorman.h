#ifndef ERRORMAN_H_
#define ERRORMAN_H_

#include "FACT.h"

static inline FACT_t
FACT_ret_error (func_t *scope, const char *desc)
{
  FACT_t ret_val;

  ret_val.type = ERROR_TYPE;
  ret_val.error.line = 0;
  ret_val.error.scope = scope;
  ret_val.error.description = (char *) desc;

  return ret_val;
}

#define FACT_throw return FACT_throw_error

FACT_INTERN_FUNC (FACT_t) FACT_throw_error (func_t *, const char *, syn_tree_t);
FACT_INTERN_FUNC (void) errorman_dump (error_t);

#endif
