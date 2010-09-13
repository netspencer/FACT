#ifndef EVAL_H_
#define EVAL_H_

/**
 * Eval.h
 * Functions for the evaluation of
 * statements and procedures. I'm
 * not sure I can explain it better
 * than that.
 *
 * TODO:
 *  - Clean it up a bit.
 *  - Remove dependency of int *
 *    in function expression.
 *
 * (c) 2010 Matthew Plant. Sorry for the bloat.
 */

extern a_type eval (func *scope, char **words);
extern a_type expression (func *scope, char **words);
extern a_type procedure (func *scope, char **words);
extern a_type lambda_proc (func *scope, char **words);

#endif
