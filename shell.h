#ifndef SHELL_H_
#define SHELL_H_

#include "common.h"

FACT_INTERN_FUNC (char *) get_input (FILE   *, unsigned int *);
FACT_INTERN_FUNC (void  ) shell     (func_t *                );

#endif
