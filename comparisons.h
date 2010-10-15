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

extern FACT_t equal (FACT_t, FACT_t);
extern FACT_t not_equal (FACT_t, FACT_t);
extern FACT_t more (FACT_t, FACT_t);
extern FACT_t more_equal (FACT_t, FACT_t);
extern FACT_t less (FACT_t, FACT_t);
extern FACT_t less_equal (FACT_t, FACT_t);
extern FACT_t and (func_t *, word_list);
extern FACT_t or (func_t *, word_list);

#endif
