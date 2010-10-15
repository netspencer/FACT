#ifndef ERRORMAN_H_
#define ERRORMAN_H_

#include "common.h"

/**
 * errorman:
 * Provides functions for the easy handling of
 * errors in code.
 *
 * (c) 2010 Matthew Plant
 */

extern FACT_t errorman_throw_reg (func_t *, char *);
extern FACT_t errorman_throw_prim (func_t *, word_list);

extern void errorman_dump (_ERROR, int, const char *);

#endif
