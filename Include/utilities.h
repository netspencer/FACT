#ifndef UTILITIES_H_
#define UTILITIES_H_

#include "FACT.h"

FACT_INTERN_FUNC (void) set_array (bool *, int);

FACT_INTERN_FUNC (int) count_until_NULL (char **);

FACT_INTERN_FUNC (bool) compare_var_arrays (var_t *, var_t *, bool);
                        
FACT_INTERN_FUNC (char * ) rm_cslashes     (char  *         );
FACT_INTERN_FUNC (char * ) combine_strs    (char  * , char *);
FACT_INTERN_FUNC (char * ) array_to_string (var_t *         );
FACT_INTERN_FUNC (char **) copy            (char  **        );

FACT_INTERN_FUNC (var_t *) string_to_array     (char * , char *     );
FACT_INTERN_FUNC (var_t *) string_array_to_var (char **, char *, int);

FACT_INTERN_FUNC (FACT_t) FACT_get_ui (unsigned int op);
FACT_INTERN_FUNC (FACT_t) FACT_get_si (  signed int op);
			  
FACT_INTERN_FUNC (int         ) tokcmp_safe (const char *, const char *, int, char *);
FACT_INTERN_FUNC (unsigned int) strcount    (const char, const char *);


/* Macro conditional tests:
 * These macros make testing of container type much cleaner.
 *  
 * Generally these would go in an if/while/for statement 
 * conditionals, e.x. if (isvar_t (x)). However, they can
 * be used otherwise.
 */

#define tokcmp(op1, op2) tokcmp_safe (op1, op2, __LINE__, __FILE__)

#define isvar_t(op)   (op.type == VAR_TYPE)
#define isfunc_t(op)  (op.type == FUNCTION_TYPE)
#define iserror_t(op) (op.type == ERROR_TYPE)

#endif