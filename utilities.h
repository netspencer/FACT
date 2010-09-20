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
extern char *array_to_string (var *);

extern var *string_to_array (char *, char *);
extern var *string_array_to_var (char **, char *, int);

#endif
