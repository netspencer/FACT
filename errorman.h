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

extern a_type errorman_throw_reg (func *, char *);
extern a_type errorman_throw_prim (func *, char **);

extern void errorman_dump (_ERROR, int, const char *);

#endif
