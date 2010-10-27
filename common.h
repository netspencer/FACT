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
#include <stdbool.h>
#include <assert.h>
#include <gmp.h>
#include <gc/gc.h>
#include <getopt.h>
#include <dlfcn.h>

/*
  The following include is an exception,
  as it is need for the typedefs below.
*/

#include "mpc_functions.h"

/*---------------------------------------------*
 * Defines: Macros for common code shared      *
 * between functions.                          *
 *---------------------------------------------*/

typedef enum _TYPE_DEFINE
  {   
    VAR_TYPE = 0,
    FUNCTION_TYPE,
    ERROR_TYPE, 
  } type_define;

/*---------------------------------------------*
 * Data types: definitions of types and        *
 * structs that are used frequently.           *
 *---------------------------------------------*/

typedef struct _VAR
{ 
  char *name;
  int array_size;
  mpc_t data;
  struct _VAR *array_up;
  struct _VAR *next; 
} var_t;

typedef struct _FUNC
{
  char *name; 
  char **args;
  char **body;

  int array_size;

  var_t *vars;

  void * (*extrn_func)(struct _FUNC *);

  struct _FUNC *funcs; 
  struct _FUNC *up;
  struct _FUNC *array_up;
  struct _FUNC *next;
} func_t;

typedef struct 
{
  func_t *scope;
  char *description;
  bool thrown;
} _ERROR;

typedef struct 
{
  type_define type;
  bool isret;
  bool break_signal;
  var_t *v_point;
  func_t *f_point;
  _ERROR error;
} FACT_t;

typedef struct
{
  bool *move_forward;
  char **syntax;
} word_list;

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
    ADD_ASSIGN,
    SUB_ASSIGN,
    MULT_ASSIGN,
    DIV_ASSIGN,
    MOD_ASSIGN,
    AT,
    SET,
    DEF,
    DEFUNC,
    FUNC_RET,
    FUNC_OBJ,
    FUNC_END,
    COMMA,
    IN_SCOPE,
    OP_CURLY,
    CL_CURLY,
    OP_BRACKET,
    NOP_BRACKET,
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
    FOR,
    THEN,
    ELSE,
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

/*---------------------------------------------*
 * Local includes: headers that provide        *
 * definitions of internal functions or        *
 * macros.                                     *
 *---------------------------------------------*/

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
