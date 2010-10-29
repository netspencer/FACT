#ifndef MANAGEMENT_H_
#define MANAGEMENT_H_

#include "common.h"

FACT_INTERN_FUNC (var_t *) alloc_var     (void            );
FACT_INTERN_FUNC (var_t *) get_local_var (func_t *, char *);
FACT_INTERN_FUNC (var_t *) get_var       (func_t *, char *);
FACT_INTERN_FUNC (var_t *) add_var       (func_t *, char *);
FACT_INTERN_FUNC (var_t *) resize_var    (var_t  *, int   );

FACT_INTERN_FUNC (func_t *) alloc_func     (void            );
FACT_INTERN_FUNC (func_t *) get_local_func (func_t *, char *);
FACT_INTERN_FUNC (func_t *) get_func       (func_t *, char *);
FACT_INTERN_FUNC (func_t *) add_func       (func_t *, char *);
FACT_INTERN_FUNC (func_t *) resize_func    (func_t *, int   );

FACT_INTERN_FUNC       (void) free_var  (var_t  *);
FACT_INTERN_FUNC       (void) free_func (func_t *);
FACT_INTERN_DEPRECATED (void) scroll    (func_t *);

#define resize_array(arg1, arg2)      resize_var  (arg1, arg2)
#define resize_func_array(arg1, arg2) resize_func (arg1, arg2)

#endif
