#ifndef MALLOC_REPLACEMENTS_H_
#define MALLOC_REPLACEMENTS_H_

#include "common.h"

FACT_INTERN_FUNC (void        ) set_bytes_used (unsigned int);
FACT_INTERN_FUNC (unsigned int) get_bytes_used (void        );

FACT_INTERN_FUNC (void *) FACT_malloc  (size_t                );
FACT_INTERN_FUNC (void *) FACT_realloc (void *, size_t, size_t);
FACT_INTERN_FUNC (void  ) FACT_free    (void *, size_t        );

#define MAX_BYTES 1000000
// #define GATES_OF_VALGRIND 

#define better_malloc              FACT_malloc
#define better_realloc(op1, op2)   FACT_realloc (op1, 0, op2)
#define better_free(op1)           FACT_free (op1, 0)

#endif

