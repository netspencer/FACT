#ifndef STRING_H_
#define STRING_H_

#include "FACT.h"

FACT_INTERN_FUNC (FACT_t) new_string      (func_t *, word_list);
FACT_INTERN_FUNC (FACT_t) print_character (func_t *, word_list);
FACT_INTERN_FUNC (FACT_t) input_character (func_t *, word_list);
FACT_INTERN_FUNC (FACT_t) print_var       (func_t *, word_list);

#endif
