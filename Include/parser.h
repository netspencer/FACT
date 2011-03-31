#ifndef PARSER_H_
#define PARSER_H_

#include "FACT.h"

FACT_INTERN_FUNC (int) op_get_prec (char *);
FACT_INTERN_FUNC (int) get_exp_length (char **, int);
FACT_INTERN_FUNC (int) get_exp_length_first (char **, int);

FACT_INTERN_FUNC (int *) get_newlines (char **);

FACT_INTERN_FUNC (char **) get_words (char *);
FACT_INTERN_FUNC (char **) parse (char **, const char *, int);

#endif
