#ifndef PARSER_H_
#define PARSER_H_

#include "common.h"

/**
 * Parser:
 * Functions for parsing text into
 * correct formating, and finding
 * the length of expressions.
 *
 * TODO:
 *  - Add another function for the
 *    replacing of things like !=
 *    with ni and such.
 *
 * (c) 2010 Matthew Plant.
 */

extern char **get_words (char *);

#if PARSING >= 2

extern void rev_shunting_yard (linked_word *);
extern char **convert_link (linked_word *);
extern void set_link (linked_word *);

extern linked_word *create_list (char **);
extern linked_word *set_list (linked_word *, word_code); 

#endif

#if PARSING < 2
extern int rev_shunting_yard (char **, int);
#endif

extern int get_exp_length (char **, int);
extern int get_exp_length_first (char **, int);

#endif
