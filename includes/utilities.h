#ifndef UTILITIES_H_
#define UTILITIES_H_

#ifdef __cplusplus
extern "C" { 
#endif

/**
 * utilities:
 * Various utilies used for making my life
 * easier, such as combining two strings,
 * and moving the string along, etc.
 *
 * (c) 2010 Matthew Plant under the GPL v3.
 * Sorry about everything.
 */

FACT_API_FUNC (char *) combine_strs (char *, char *);
FACT_API_FUNC (char **) copy (char **);
FACT_API_FUNC (char *) array_to_string (var_t *);

FACT_API_FUNC (var_t *) string_to_array (char *, char *);
FACT_API_FUNC (var_t *) string_array_to_var (char **, char *, int);

FACT_API_FUNC (FACT_t) FACT_get_ui (unsigned int op);
FACT_API_FUNC (FACT_t) FACT_get_si (signed int op);
  
#ifdef __cplusplus
}
#endif

/*
  Macro conditional tests:
  These macros make testing of container types
  much cleaner.

  These would generally go in if statement
  conditionals, e.x. if (isvar_t (x)), however
  they can be used otherwise.
*/

#define isvar_t(op)   (op.type == VAR_TYPE)
#define isfunc_t(op)  (op.type == FUNCTION_TYPE)
#define iserror_t(op) (op.type == ERROR_TYPE)

#define FACT_conv_ui(op)  mpc_get_ui (op.v_point->data)
#define FACT_conv_si(op)  mpc_get_si (op.v_point->data)
#define FACT_conv_str(op) mpc_get_str (op.v_point->data)

#define var_get_ui(op)  mpc_get_ui (op->data)
#define var_get_si(op)  mpc_get_si (op->data)
#define var_get_str(op) mpc_get_str (op->data)

#endif
