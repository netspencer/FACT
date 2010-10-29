#ifndef COMMON_H_
#define COMMON_H_

/**
 * common.h:
 * The main header file. Includes all
 * the library headers, macro and
 * struct definitions, and typedefs.
 *
 * Now let's do that Allegro thing:
 *    ________   ________   ________   _________
 *   /\  _____\ /\  ___  \ /\  _____\ /\___  ___\
 *   \ \ \____/ \ \ \__L\ \\ \ \____/ \/__/\ \__/
 *    \ \  ___\  \ \  ____ \\ \ \         \ \ \
 *     \ \ \__/   \ \ \__/\ \\ \ \______   \ \ \
 *      \ \_\      \ \_\ \ \_\\ \_______\   \ \_\
 *       \/_/       \/_/  \/_/ \/_______/    \/_/
 *
 * (C) 2010 Matthew Plant. 
 */

#define FACT_INTERN_FUNC(type) extern type 
#if (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#define FACT_INTERN_DEPRECATED(type) extern __attribute__((__deprecated__)) type
#else
#define FACT_INTERN_DEPRECATED(type) extern type
#endif

/*---------------------------------------------*
 * Library includes: headers that are not part *
 * of the program itself.                      *
 *---------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>
#include <gmp.h>
#include <gc/gc.h>
#include <getopt.h>
#include <dlfcn.h>

/*---------------------------------------------*
 * Local includes: headers that provide        *
 * definitions of internal functions macros or *
 * data structures.                            *
 *---------------------------------------------*/

#include "typedefs.h"
#include "mpc_functions.h"
#include "utilities.h"
#include "modules.h"
#include "malloc_replacements.h"
#include "errorman.h"
#include "management.h"
#include "shell.h"
#include "parser.h"
#include "eval.h"
#include "mem.h"
#include "functions.h"
#include "math.h"
#include "comparisons.h"
#include "string.h"
#include "statements.h"
#include "execfile.h"
#include "primitives.h"

#endif
