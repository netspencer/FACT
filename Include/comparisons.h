#ifndef COMPARISONS_H_
#define COMPARISONS_H_

#include "FACT.h"

FACT_INTERN_FUNC (FACT_t) equal      (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) not_equal  (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) more       (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) more_equal (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) less       (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) less_equal (FACT_t, FACT_t);

FACT_INTERN_FUNC (FACT_t) and (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) or  (func_t *, syn_tree_t);

#endif
