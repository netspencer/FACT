#ifndef STATEMENTS_H_
#define STATEMENTS_H_

#include "common.h"

/**
 * statements:
 * Provides functions for if
 * statements and while and for
 * loops.
 */

FACT_INTERN_FUNC (FACT_t) invalid_if    (func_t *, word_list);
FACT_INTERN_FUNC (FACT_t) invalid_else  (func_t *, word_list);
FACT_INTERN_FUNC (FACT_t) invalid_while (func_t *, word_list);
FACT_INTERN_FUNC (FACT_t) invalid_for   (func_t *, word_list);

FACT_INTERN_FUNC (FACT_t) if_statement (func_t *, word_list, bool *);
FACT_INTERN_FUNC (FACT_t) else_clause  (func_t *, char **          );
FACT_INTERN_FUNC (FACT_t) while_loop   (func_t *, char **          );
FACT_INTERN_FUNC (FACT_t) for_loop     (func_t *, char **          );
 
#endif
