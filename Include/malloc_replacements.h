#ifndef MALLOC_REPLACEMENTS_H_
#define MALLOC_REPLACEMENTS_H_

#include "FACT.h"

FACT_INTERN_FUNC (void *) FACT_malloc  (size_t);
FACT_INTERN_FUNC (void *) FACT_realloc (void *, size_t);
FACT_INTERN_FUNC (void  ) FACT_free    (void *);

#define better_malloc              FACT_malloc
#define better_realloc(op1, op2)   FACT_realloc (op1, op2)
#define better_free(op1)           FACT_free (op1, 0)

#endif

