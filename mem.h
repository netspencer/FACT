#ifndef MEM_H_
#define MEM_H_

/**
 * Mem.h:
 * Functions for the creation
 * and handeling of variables
 * and objects.
 *
 * TODO:
 *  - Fix set for functions.
 *  - Clean up some stuff.
 *  - Add delete functions.
 *
 * (c) 2010 Matthew Plant. Sorry for the bloat.
 */

extern a_type get_array (var *, func *, char **);
extern a_type get_array_size (func *, char **);
extern a_type define (func *, char **);
extern a_type defunc (func *, char **);
extern a_type set (func *, char **);
extern a_type return_array (func *, char **); 
extern a_type size_of (func *, char **);
extern a_type get_array_var (var *, func *, char **);
extern a_type get_array_func (func *, func *, char **);

extern var *clone_var (var *, char *);

#endif
