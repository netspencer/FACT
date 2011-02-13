#ifndef ERRORMAN_H_
#define ERRORMAN_H_

#include "FACT.h"

FACT_INTERN_FUNC (FACT_t) errorman_throw_reg        (func_t *, char *);
FACT_INTERN_FUNC (FACT_t) errorman_throw_catchable  (func_t *, char *);
			  
FACT_INTERN_FUNC (void) errorman_dump (_ERROR);

#endif
