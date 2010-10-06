#ifndef COMPARISONS_H_
#define COMPARISONS_H_

/**
 * comparisons:
 * Provides logical and comparison
 * operators, such as more than,
 * less than, and, or, etc.
 *
 * Linked from Operators.c
 *
 * (c) 2010 Matthew PLant. Sorry for the bloat.
 */

/*
  I don't think I'm going to explain
  every single function here. You get
  the idea from math.h.
*/

extern a_type equal (a_type, a_type);
extern a_type not_equal (a_type, a_type);
extern a_type more (a_type, a_type);
extern a_type more_equal (a_type, a_type);
extern a_type less (a_type, a_type);
extern a_type less_equal (a_type, a_type);
//extern a_type and (a_type, a_type);
extern a_type and (func *, char **);
extern a_type or (func *, char **);//a_type, a_type);

#endif
