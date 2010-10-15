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

extern FACT_t liven_func (func_t *, word_list);
extern FACT_t run_func (func_t *, word_list);
extern FACT_t new_scope (func_t *, word_list);
extern FACT_t in_scope (func_t *, word_list);
extern FACT_t lambda (func_t *, word_list);

#endif
