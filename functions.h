#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

/**
 * functions:
 * Provide functions for the creation
 * and running of, well, functions.
 *
 * TODO:
 *  - Some cleaning up might be nice.
 *
 * (c) 2010 Matthew Plant - sorry for the bloat.
 */

extern a_type add_func (func *, char **);
extern a_type run_func (func *, char **);
extern a_type new_scope (func *, char **);
extern a_type in_scope (func *, char **);

#endif
