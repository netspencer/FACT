#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#include "common.h"

/* Check to see if a token is a valid primitive
 * (this will be removed in the future, so don't
 * rely on it).
 */
FACT_INTERN_FUNC (int) isprim (char *);

/* Initialize the built-in functions. */
FACT_INTERN_FUNC (void) init_BIFs (func_t *);

/* Run a primitive or evaluate math */
FACT_INTERN_FUNC (FACT_t) run_prim (func_t *, word_list, int);
FACT_INTERN_FUNC (FACT_t) eval_math (func_t *, word_list, int);

#endif
