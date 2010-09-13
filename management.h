#ifndef MANAGEMENT_H_
#define MANAGEMENT_H_

#include "common.h"

#define getVar(arg1, arg2) get_var (arg1, arg2)
#define getFunc(arg1, arg2) get_func (arg1, arg2)

extern var *alloc_var ();
extern func *alloc_func ();
extern var *get_local_var (func *, char *);
extern func *get_local_func (func *, char *);
extern var *get_var (func *, char *);
extern func *get_func (func *, char *);

extern void scroll (func *);

extern var *add_var (func *, char *);
extern func *addFunc (func *, char *);

#define resize_array(arg1, arg2) resize_var (arg1, arg2)
#define resize_func_array(arg1, arg2) resize_func (arg1, arg2);

extern var *resize_var (var *, int);
extern func *resize_func (func *, int);

extern void free_var (var *);
extern void free_func (func *);

/* DEPRECATED:
extern unsigned char *charToChar (char);
extern unsigned char *intToChar (int);
extern unsigned char *longToChar (long);
extern unsigned char *quadToChar (long long);

extern int varToChar (struct var);
extern int varToInt (struct var);
extern long varToLong (struct var);
extern long long varToQuad (struct var);
*/

#endif
