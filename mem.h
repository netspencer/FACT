#ifndef MEM_H_
#define MEM_H_

/**
 * mem:
 * Functions for the creation
 * and handeling of var_tiables
 * and objects.
 *
 * TODO:
 *  - Fix set for functions.
 *  - Clean up some stuff.
 *  - Add delete functions.
 *
 * (c) 2010 Matthew Plant. Sorry for the bloat.
 */

//extern FACT_t get_array (var_t *, func_t *, char **);
extern FACT_t get_array_size (func_t *, word_list);
extern FACT_t define (func_t *, word_list);
extern FACT_t defunc (func_t *, word_list);
extern FACT_t set (func_t *, word_list);
extern FACT_t return_array (func_t *, word_list); 
extern FACT_t size_of (func_t *, word_list);
extern FACT_t get_array_var_t (var_t *, func_t *, word_list);
extern FACT_t get_array_func_t (func_t *, func_t *, word_list);

extern var_t *clone_var_t (var_t *, char *);

#endif
