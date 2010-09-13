#ifndef OPERATORS_H_
#define OPERATORS_H_

/**
 * Operators.h
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

extern a_type equal (a_type arg1, a_type arg2);
extern a_type not_equal (a_type arg1, a_type arg2);
extern a_type more (a_type arg1, a_type arg2);
extern a_type more_equal (a_type arg1, a_type arg2);
extern a_type less (a_type arg1, a_type arg2);
extern a_type less_equal (a_type arg1, a_type arg2);
extern a_type and (a_type arg1, a_type arg2);
extern a_type or (a_type arg1, a_type arg2);

#endif
