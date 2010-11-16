#ifndef PARSER_H_
#define PARSER_H_

#include "common.h"

FACT_INTERN_FUNC (void) rev_shunting_yard (linked_word *);
FACT_INTERN_FUNC (void) set_link          (linked_word *);

FACT_INTERN_FUNC (int) get_exp_length       (char **, int);
FACT_INTERN_FUNC (int) get_exp_length_first (char **, int);

FACT_INTERN_FUNC (char *) parsing_error_set_get (char        *                     );
FACT_INTERN_FUNC (bool  ) parsing_error         (linked_word *, bool, unsigned char);

FACT_INTERN_FUNC (char **) get_words    (char        *);
FACT_INTERN_FUNC (char **) convert_link (linked_word *);

FACT_INTERN_FUNC (linked_word *) create_list (char        **          );
FACT_INTERN_FUNC (linked_word *) set_list    (linked_word *, word_code); 

#define parsing_get_error() parsing_error_set_get (NULL)

#endif
