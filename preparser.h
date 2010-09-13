#ifndef PREPARSER_H_
#define PREPARSER_H_

#include "interpreter.h"

/**
 * Preparser:
 * Functions and typedefs that configure
 * a string of characters as to easily
 * be able to go through the rev-shunting-
 * yard function. Eventually the data
 * structures will be implemented to be
 * used by the entire program, cleaning
 * things up a bit.
 *
 * TODO:
 *  - Implement the data structure to be
 *    used in the entire program. This
 *    will clean things up a bit.
 *
 * (c) 2010 Matthew Plant.
 */

enum codes {
  PARSING_ERROR = -1,
  UNKNOWN, /* 0 */
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
  RETURN
};

extern linked_word *create_links (char **, char end);

#endif
