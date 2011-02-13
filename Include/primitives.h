#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#include "FACT.h"

FACT_INTERN_FUNC (int) isprim (char *);

FACT_INTERN_FUNC (void) init_BIFs (func_t *);

FACT_INTERN_FUNC (FACT_t) run_prim  (func_t *, word_list, int);
FACT_INTERN_FUNC (FACT_t) eval_math (func_t *, word_list, int);

#endif
