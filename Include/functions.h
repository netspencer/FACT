#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "FACT.h"

FACT_INTERN_FUNC (FACT_t) liven_func (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) run_func   (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) new_scope  (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) in_scope   (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) lambda     (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) up         (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) this       (func_t *, syn_tree_t); 
FACT_INTERN_FUNC (FACT_t) NULL_func  (func_t *, syn_tree_t);

#endif
