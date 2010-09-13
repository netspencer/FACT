#ifndef GMP_MALLOCS_H_
#define GMP_MALLOCS_H_

#include "common.h"

/**
 * gmp_allocs:
 * These are a bunch of replacement function
 * for the gmp allocators that introduce
 * garbage collection. Sorry for the bloat.
 *
 * (c) 2010 Matthew Plant. Copyleft.
 */

extern void *gmp_malloc (size_t);
extern void *gmp_realloc (void *, size_t, size_t);
extern void gmp_free (void *, size_t);

#endif

