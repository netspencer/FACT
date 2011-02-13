#ifndef MANAGEMENT_H_
#define MANAGEMENT_H_

#ifdef __cplusplus
extern "C" {
#endif

FACT_API_FUNC (var_t *) alloc_var     ( void );
FACT_API_FUNC (var_t *) get_local_var (func_t *, char *);
FACT_API_FUNC (var_t *) get_var       (func_t *, char *);
FACT_API_FUNC (var_t *) add_var       (func_t *, char *);
FACT_API_FUNC (var_t *) resize_var    (var_t  *, int);
  
FACT_API_FUNC (func_t *) alloc_func     ( void );
FACT_API_FUNC (func_t *) get_local_func (func_t *, char *);
FACT_API_FUNC (func_t *) get_func       (func_t *, char *);
FACT_API_FUNC (func_t *) add_func       (func_t *, char *);
FACT_API_FUNC (func_t *) resize_func    (func_t *, int);
  
FACT_API_DEPRECATED ( void ) scroll (func_t *);

FACT_API_FUNC ( void ) free_var  (var_t  *);
FACT_API_FUNC ( void ) free_func (func_t *);

#ifdef __cplusplus
}
#endif

#define resize_array(arg1, arg2)      resize_var  (arg1, arg2)
#define resize_func_array(arg1, arg2) resize_func (arg1, arg2)

#endif
