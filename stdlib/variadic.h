#ifndef VARIADIC_H_
#define VARIADIC_H_

#include "stdlib.h"

FACT_API_FUNC (FACT_t) get_arg_type (func_t *);		       
FACT_API_FUNC (FACT_t) get_arg_num  (func_t *);
FACT_API_FUNC (FACT_t) get_arg      (func_t *);

#endif
