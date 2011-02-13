#ifndef ERRORMAN_H_
#define ERRORMAN_H_

#include "FACT.h"

FACT_API_FUNC (FACT_t) errorman_throw_reg        (func_t *, char *           );
FACT_API_FUNC (FACT_t) errorman_throw_catchable  (func_t *, char *           );
FACT_API_FUNC (void  ) errorman_dump             (_ERROR  , int, const char *);

#endif
