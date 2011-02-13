#ifndef MALLOC_REPLACEMENTS_H_
#define MALLOC_REPLACEMENTS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * mallocs_replacements:
 * These are a bunch of replacement function
 * for the gmp allocators that introduce
 * garbage collection. Sorry for the bloat.
 *
 * (c) 2010 Matthew Plant. GPL v.3 
 */

#define MAX_BYTES 1000000

FACT_API_FUNC (void) set_bytes_used (unsigned int);
FACT_API_FUNC (unsigned int) get_bytes_used ( void );

FACT_API_FUNC (void *) FACT_malloc (size_t);
FACT_API_FUNC (void *) FACT_realloc (void *, size_t, size_t);
FACT_API_FUNC (void) FACT_free (void *, size_t);

#ifdef __cplusplus
}
#endif

#define better_malloc(op) FACT_malloc (op)
#define better_realloc(op1, op2) FACT_realloc (op1, 0, op2)
#define better_free(op1, op2) FACT_free (op1, op2)

#endif

