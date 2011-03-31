#ifndef STATEMENTS_H_
#define STATEMENTS_H_

#include "FACT.h"

FACT_INTERN_FUNC (FACT_t) if_statement (func_t *, syn_tree_t, bool *);
FACT_INTERN_FUNC (FACT_t) error        (func_t *, syn_tree_t, bool *);
FACT_INTERN_FUNC (FACT_t) else_clause  (func_t *, syn_tree_t        );
FACT_INTERN_FUNC (FACT_t) while_loop   (func_t *, syn_tree_t        );
FACT_INTERN_FUNC (FACT_t) for_loop     (func_t *, syn_tree_t        );
 
#endif
