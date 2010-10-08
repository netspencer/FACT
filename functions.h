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

extern a_type liven_func (func *, word_list);
extern a_type run_func (func *, word_list);
extern a_type new_scope (func *, word_list);
extern a_type in_scope (func *, word_list);
extern a_type lambda (func *, word_list);

#endif
