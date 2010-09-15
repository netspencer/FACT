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
#include <malloc.h>
#include <stdbool.h>
#include <assert.h>
#include <gmp.h>
#include <gc/gc.h>
#include <getopt.h>

/* global variables: */

/*
  bytes_used is sort of a ticker that tracks
  the amount of bytes in use. When it exceeds
  a certain limit, garbage collections is
  forced. I implemented it unaware of the
  various methods GC has to circumvent it,
  but it is still used.
*/

extern unsigned int bytes_used;

/*
  mem_trackopt is a boolean variable used to
  track the heap size each time something is
  allocated.
*/

extern bool mem_trackopt;

/*---------------------------------------------*
 * Defines: Macros for common code shared      *
 * between functions.                          *
 *---------------------------------------------*/

enum {   
  VAR_TYPE,
  FUNCTION_TYPE,
  ERROR_TYPE
};

#define MAX_BYTES 10000000

//#define PARSING 2

/*
  new_malloc: addition to regular malloc that
  reports and error and exits whenever malloc
  returns a NULL pointer.
*/

static inline void *better_malloc (size_t alloc_size)
{
  void *temp_pointer;

  if (mem_trackopt)
    printf("Heap size = %d\n", (int) GC_get_heap_size());

  
  if (bytes_used >= MAX_BYTES)
    {
      GC_gcollect ();
      bytes_used = 0;
    }

  temp_pointer = GC_malloc (alloc_size);

  bytes_used += alloc_size;

  if (temp_pointer == NULL)
    {
      GC_gcollect ();
      temp_pointer = GC_malloc (alloc_size);

      if (temp_pointer == NULL)
	{
	  fprintf (stderr, "Could not allocate block of size %d, exiting.\n", (int) alloc_size);
	  exit (EXIT_FAILURE);
	}
    }

  return temp_pointer;
}

/*
  new_realloc: like new_malloc except exits and
  reports when realloc returns returns a null
  pointer.
*/

static inline void *better_realloc (void *to_resize, size_t alloc_size)
{
  void *temp_pointer;
  
  if (mem_trackopt)
    printf("Heap size = %d\n", (int) GC_get_heap_size());

  if (bytes_used >= MAX_BYTES)
    {
      GC_gcollect ();
      bytes_used = 0;
    }

  temp_pointer = GC_realloc (to_resize, alloc_size);

  bytes_used += alloc_size;

  if (temp_pointer == NULL)
    {
      GC_gcollect ();
      temp_pointer = GC_realloc (to_resize, alloc_size);

      if (temp_pointer == NULL)
	{
	  fprintf (stderr, "Could not allocate block of size %d, exiting.\n", (int) alloc_size);
	  exit (EXIT_FAILURE);
	}
    }
  
  return temp_pointer;
}

/*---------------------------------------------*
 * Data types: definitions of types and        *
 * structs that are used frequently.           *
 *---------------------------------------------*/

typedef struct _VAR
{ 
  char *name;
  int array_size;
  mpz_t data;
  struct _VAR *array_up;
  struct _VAR *next; 
} var;

typedef struct _FUNC
{
  char *name; 
  char **args;
  char **body;
  int array_size;
  var *vars; 
  struct _FUNC *funcs; 
  struct _FUNC *up;
  struct _FUNC *array_up;
  struct _FUNC *next;
} func;

typedef struct 
{
  func *scope;
  char *description;
} _ERROR;

typedef struct 
{
  char type;
  bool isret;
  var *v_point;
  func *f_point;
  _ERROR error;
} a_type;

#if PARSING >= 2
typedef enum _word_codes
  {
    PARSING_ERROR = -1,
    END,                     /* -1 */
    UNKNOWN,                 /* 0  */
    PLUS,                    
    MINUS,
    MULTIPLY,
    DIVIDE,
    MOD,
    AT,
    SET,
    DEF,
    DEFUNC,
    FUNC_RET,
    FUNC_OBJ,
    COMMA,
    IN_SCOPE,
    OP_CURLY,
    CL_CURLY,
    OP_BRACKET,
    CL_BRACKET,
    OP_PAREN,
    CL_PAREN,
    QUOTE,
    AND,
    OR,
    EQ,
    NEQ,
    LESS,
    MORE,
    LESS_EQ,
    MORE_EQ,
    SIZE,
    IF,
    WHILE,
    SEMI,
    RETURN_STAT,
  } word_code;

typedef struct _LINKED_WORD
{
  word_code code;
  char *physical; /* only used if code is equal to UNKNOWN, otherwise it is NULL. */
  struct _LINKED_WORD *hidden;
  struct _LINKED_WORD *hidden_up;
  struct _LINKED_WORD *next;
  struct _LINKED_WORD *previous;
} linked_word;
#endif


/*---------------------------------------------*
 * Local includes: headers that provide        *
 * definitions of internal functions or        *
 * macros.                                     *
 *---------------------------------------------*/

#include "utilities.h"
#include "gmp_mallocs.h"
#include "garbage.h"
#include "errorman.h"
#include "management.h"
#include "shell.h"
#include "parser.h"
#include "eval.h"
#include "mem.h"
#include "functions.h"
#include "math.h"
#include "operators.h"
#include "string.h"
#include "statements.h"
#include "execfile.h"
#include "primitives.h"

#endif
