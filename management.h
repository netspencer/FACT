#ifndef MANAGEMENT_H_
#define MANAGEMENT_H_

#include "common.h"

#define getVar(arg1, arg2) get_var (arg1, arg2)
#define getFunc(arg1, arg2) get_func (arg1, arg2)

extern var_t *alloc_var ();
extern func_t *alloc_func ();
extern var_t *get_local_var (func_t *, char *);
extern func_t *get_local_func (func_t *, char *);
extern var_t *get_var (func_t *, char *);
extern func_t *get_func (func_t *, char *);

extern void scroll (func_t *);

extern var_t *add_var (func_t *, char *);
extern func_t *add_func (func_t *, char *);

#define resize_array(arg1, arg2) resize_var (arg1, arg2)
#define resize_func_array(arg1, arg2) resize_func (arg1, arg2);

extern var_t *resize_var (var_t *, int);
extern func_t *resize_func (func_t *, int);

extern void free_var (var_t *);
extern void free_func (func_t *);

/* DEPRECATED:
extern unsigned char *charToChar (char);
extern unsigned char *intToChar (int);
extern unsigned char *longToChar (long);
extern unsigned char *quadToChar (long long);

extern int var_tToChar (struct var_t);
extern int var_tToInt (struct var_t);
extern long var_tToLong (struct var_t);
extern long long var_tToQuad (struct var_t);
*/

#endif
