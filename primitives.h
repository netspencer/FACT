#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#include "common.h"

/**
 * primitives:
 * Provides functions for the
 * easy handeling and calling
 * of math calls and primitives.
 * This API needs to be fixed A LOT.
 */

FACT_INTERN_FUNC (int) isprim     (char *);

FACT_INTERN_FUNC (void) add_prim       (const char *, FACT_t (*)(func_t *, word_list));
FACT_INTERN_FUNC (void) init_std_prims (func_t     *                                 );

FACT_INTERN_FUNC (FACT_t) runprim   (func_t *, word_list, int);
FACT_INTERN_FUNC (FACT_t) eval_math (func_t *, word_list, int);

#endif
