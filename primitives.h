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

extern a_type runprim (func *scope, char **words);
extern a_type eval_math (func *scope, char **words);

#endif
