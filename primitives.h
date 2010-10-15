#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

/**
 * Primitives.h:
 * Provides functions for the
 * easy handeling and calling
 * of math calls and primitives.
 *
 * TODO:
 *  - Remove unnecessary crap.
 *
 * (c) 2010 Matthew Plant.
 */

extern int isprim (char *);
extern int ismathcall (char *);

extern void add_prim (const char *, FACT_t (*)(func_t *, word_list));
extern void init_std_prims ();

extern FACT_t runprim (func_t *, word_list, int);
extern FACT_t eval_math (func_t *, word_list, int);

#endif
