#ifndef ERRORMAN_H_
#define ERRORMAN_H_

#include "common.h"

/**
 * errorman.c:
 * Provides functions for the easy handling of
 * errors in code.
 *
 * (c) 2010 Matthew Plant
 */

extern a_type errorman_throw_reg (func *, char *);

extern void errorman_dump (_ERROR, int, const char *);

enum error_calls {
  NON = 0, /* not an error */
  CVTF,
  CFTV,
  RV,
  RF,
  LESSPRIM,
  LESSFUNC,
  MOREPRIM,
  MOREFUNC,
  INVALPRIM,
  INVALFUNC,
  ESCAPARR,
  DIVNON,
  MODNON,
  SYNTAX
};

#endif
