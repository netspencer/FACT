/* This file is part of FACT.
 *
 * FACT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FACT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FACT. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef UTILITIES_H_
#define UTILITIES_H_

#ifdef __cplusplus
extern "C" { 
#endif

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
