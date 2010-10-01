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

extern void add_prim (const char *, a_type (*)(func *, char **));
extern void init_std_prims ();

extern a_type runprim (func *, char **, int);
extern a_type eval_math (func *, char **, int);

#endif
