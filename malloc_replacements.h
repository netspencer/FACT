#ifndef MALLOC_REPLACEMENTS_H_
#define MALLOC_REPLACEMENTS_H_

#include "common.h"

/**
 * mallocs_replacements:
 * These are a bunch of replacement function
 * for the gmp allocators that introduce
 * garbage collection. Sorry for the bloat.
 *
 * (c) 2010 Matthew Plant. Copyleft.
 */

#define MAX_BYTES 1000000

extern void set_bytes_used (unsigned int);
extern unsigned int get_bytes_used ( void );

extern void *FACT_malloc (size_t);
extern void *FACT_realloc (void *, size_t, size_t);
extern void FACT_free (void *, size_t);

#define better_malloc(op) FACT_malloc (op)
#define better_realloc(op1, op2) FACT_realloc (op1, 0, op2)
#define better_free(op1, op2) FACT_free (op1, op2)

#endif

