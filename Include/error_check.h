#ifndef ERROR_CHECK_H_
#define ERROR_CHECK_H_

#include "FACT.h"

FACT_INTERN_FUNC (void) print_parsing_error     (const char  *, int                      );
FACT_INTERN_FUNC (bool) check_for_incompletions (const char  *, char *                   );			  
FACT_INTERN_FUNC (bool) check_for_errors        (linked_word *, unsigned int, int *, bool);

#endif
