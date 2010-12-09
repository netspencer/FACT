#ifndef MATH_H_
#define MATH_H_

#include "common.h"

FACT_INTERN_FUNC (bool  ) isnum      (char *);
FACT_INTERN_FUNC (FACT_t) num_to_var (char *);      /* converts a string to a var_t. */

FACT_INTERN_FUNC (FACT_t) add    (FACT_t, FACT_t);  /* adds two numbers together     */
FACT_INTERN_FUNC (FACT_t) sub    (FACT_t, FACT_t);  /* subtracts two numbers         */
FACT_INTERN_FUNC (FACT_t) mult   (FACT_t, FACT_t);  /* multiplies two numbers        */
FACT_INTERN_FUNC (FACT_t) divide (FACT_t, FACT_t);  /* divides two numbers           */
FACT_INTERN_FUNC (FACT_t) mod    (FACT_t, FACT_t);  /* modulos two numbers           */
FACT_INTERN_FUNC (FACT_t) bit_and (FACT_t, FACT_t); 
FACT_INTERN_FUNC (FACT_t) bit_ior (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) bit_xor (FACT_t, FACT_t);

FACT_INTERN_FUNC (FACT_t) add_assignment  (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) sub_assignment  (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) mult_assignment (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) div_assignment  (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) mod_assignment  (FACT_t, FACT_t);

FACT_INTERN_FUNC (FACT_t) negative  (func_t *, word_list);
FACT_INTERN_FUNC (FACT_t) paren     (func_t *, word_list);

#endif
