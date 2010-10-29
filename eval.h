#ifndef EVAL_H_
#define EVAL_H_

#include "common.h"

/**
 * eval:
 * Functions for the evaluation of
 * statements and procedures. I'm
 * not sure I can explain it better
 * than that.
 */

FACT_INTERN_FUNC (FACT_t) eval        (func_t *, word_list);
FACT_INTERN_FUNC (FACT_t) expression  (func_t *, char **  );
FACT_INTERN_FUNC (FACT_t) procedure   (func_t *, char **  );
FACT_INTERN_FUNC (FACT_t) lambda_proc (func_t *, word_list);

#endif
