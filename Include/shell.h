#ifndef SHELL_H_
#define SHELL_H_

#include "FACT.h"

FACT_INTERN_FUNC (char *) get_input (FILE *, unsigned int *, const char *, const char *);
FACT_INTERN_FUNC (void  ) shell (func_t *);

#endif