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

extern FACT_t new_string (func_t *, word_list);
extern FACT_t print_character (func_t *, word_list);
extern FACT_t input_character (func_t *, word_list);
extern FACT_t print_var (func_t *, word_list);

#endif
