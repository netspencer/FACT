#ifndef STRING_H_
#define STRING_H_

/**
 * String.h:
 * Provides functions for the creation
 * of strings, printing of characters,
 * and the getting of input.
 *
 * Linked from: String.c
 *
 * TODO:
 *  - Remove support for print_character
 *    and input_character as soon as
 *    library support is added.
 *
 * (c) 2010 Matthew Plant. Sorry for the bloat.
 */

extern a_type new_string (func *, word_list);
extern a_type print_character (func *, word_list);
extern a_type input_character (func *, word_list);
extern a_type print_var (func *, word_list);

#endif
