#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "common.h"

/**
 * utilities:
 * Various utilies used for making my life
 * easier, such as combining two strings,
 * and moving the string along, etc.
 *
 * (c) 2010 Matthew Plant
 * Sorry about everything.
 *
 */

extern char *combine_strs (char *, char *);
extern char **copy (char **);
extern char *array_to_string (var_t *);

extern var_t *string_to_array (char *, char *);
extern var_t *string_array_to_var_t (char **, char *, int);

extern FACT_t FACT_get_ui (unsigned int op);
extern FACT_t FACT_get_si (signed int op);

/* The following describes macros to make everyone's lives
   a bit clearner. */

/*
  Macro conditional tests:
  These macros make testing of container types
  much cleaner.

  These would generally go in if statement
  conditionals, e.x. if (isvar_t (x)), however
  they can be used otherwise.
*/

#define isvar_t(op) op.type == VAR_TYPE
#define isfunc_t(op) op.type == FUNCTION_TYPE
#define iserror_t(op) op.type == ERROR_TYPE

#endif
