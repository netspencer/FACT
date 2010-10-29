#ifndef ERRORMAN_H_
#define ERRORMAN_H_

#include "common.h"

FACT_INTERN_FUNC (FACT_t) errorman_throw_reg  (func_t *, char *   );
FACT_INTERN_FUNC (FACT_t) errorman_throw_prim (func_t *, word_list);

FACT_INTERN_FUNC (void) errorman_dump (_ERROR, int, const char *);

#endif
