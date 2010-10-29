#ifndef STRING_H_
#define STRING_H_

#include "common.h"

/**
 * string:
 * Provides functions for the creation
 * of strings, printing of characters,
 * and the getting of input.
 *
 * TODO:
 *  - Remove support for print_character
 *    and input_character as soon as
 *    library support is added.
 *  - Add support for single quotes. 
 */

FACT_INTERN_FUNC (FACT_t) new_string      (func_t *, word_list);
FACT_INTERN_FUNC (FACT_t) print_character (func_t *, word_list);
FACT_INTERN_FUNC (FACT_t) input_character (func_t *, word_list);
FACT_INTERN_FUNC (FACT_t) print_var       (func_t *, word_list);

#endif
