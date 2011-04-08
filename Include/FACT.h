#ifndef FACT_H_
#define FACT_H_

/**
 * FACT.h:
 * The main header file. Includes all
 * the library headers, macro and
 * struct definitions, and typedefs.
 *
 *    ________  ________   ________  _________
 *   /\  _____\/\  ___  \ /\  _____\/\___  ___\
 *   \ \ \____/\ \ \__L\ \\ \ \____/\/__/\ \__/
 *    \ \  ___\ \ \  ____ \\ \ \        \ \ \
 *     \ \ \__/  \ \ \__/\ \\ \ \______  \ \ \
 *      \ \_\     \ \_\ \ \_\\ \_______\  \ \_\
 *       \/_/      \/_/  \/_/ \/_______/   \/_/
 *
 * (C) 2010, 2011 Matthew Plant. 
 */

///////////////////////
// Macro definitions.
///////////////////////

#define FACT_INTERN_FUNC(type) type
#define FACT_INLINE_FUNC(type) inline type 

#if (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
# define FACT_INTERN_DEPRECATED(type) __attribute__((__deprecated__)) type
#else
# define FACT_INTERN_DEPRECATED(type) type
#endif /* (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)) */

#define FACT_DEFINE_BIF(op1, op2) FACT_t op1##_func (func_t * scope);   \
  BIF op1##_BIF = {                                                     \
    .name = #op1,                                                       \
    .arguments = op2 " ",						\
    .function = (void * (*) (struct FACT_func *)) &(op1##_func)         \
  };									\
  FACT_t op1##_func (func_t * scope)

#define FACT_EXTERN_BIF(name) BIF name ## _BIF
#define FACT_BIF(name) name ## _BIF

#define BYTES_IN_POINTER (sizeof (void *) / sizeof (char))
#define SAFE // If this is turned on, pointers value will be checked before being dereferenced. 
#define PARSE_CHECK
#define DEBUG
#define GC_THREADS // So that GC is reentrant.

/////////////////////////////
// Library header includes.
/////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <endian.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include <gmp.h>
#include <pthread.h>
#include <gc/gc.h>
#include <getopt.h>
#include <dlfcn.h>
#include <errno.h>

////////////////////////////
// Source header includes.
////////////////////////////

#include "typedefs.h"
#include "mpc_functions.h"
#include "utilities.h"
#include "malloc_replacements.h"
#include "errorman.h"
#include "management.h"
#include "BIFs.h"
#include "variadic.h"
#include "modules.h"
#include "bytecode.h"
#include "shell.h"
#include "parser.h"
#include "sprout.h"
#include "eval.h"
#include "mem.h"
#include "functions.h"
#include "FACT_math.h"
#include "comparisons.h"
#include "statements.h"
#include "execfile.h"
#include "primitives.h"

#endif
