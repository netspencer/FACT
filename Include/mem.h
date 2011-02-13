#ifndef MEM_H_
#define MEM_H_

#include "FACT.h"

FACT_INTERN_FUNC (FACT_t) get_array_size  (func_t *, word_list          );
FACT_INTERN_FUNC (FACT_t) define          (func_t *, word_list          );
FACT_INTERN_FUNC (FACT_t) defunc          (func_t *, word_list          );
FACT_INTERN_FUNC (FACT_t) set             (func_t *, word_list          );
FACT_INTERN_FUNC (FACT_t) return_array    (func_t *, word_list          ); 
FACT_INTERN_FUNC (FACT_t) size_of         (func_t *, word_list          );
FACT_INTERN_FUNC (FACT_t) get_array_func  (func_t *, func_t *, word_list);
FACT_INTERN_FUNC (FACT_t) get_array_var   (var_t  *, func_t *, word_list);
FACT_INTERN_FUNC (FACT_t) combine_arrays  (FACT_t  , FACT_t             );

FACT_INTERN_FUNC (var_t *) clone_var (var_t *, char *);

#endif
