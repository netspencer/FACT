#ifndef STATEMENTS_H_
#define STATEMENTS_H_

#include "FACT.h"

FACT_INTERN_FUNC (FACT_t) if_statement (func_t *, word_list, bool *);
FACT_INTERN_FUNC (FACT_t) on_error     (func_t *, word_list, bool *);
FACT_INTERN_FUNC (FACT_t) else_clause  (func_t *, word_list        );
FACT_INTERN_FUNC (FACT_t) while_loop   (func_t *, word_list        );
FACT_INTERN_FUNC (FACT_t) for_loop     (func_t *, word_list        );
 
#endif