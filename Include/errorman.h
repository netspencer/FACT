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

#ifndef ERRORMAN_H_
#define ERRORMAN_H_

#include "FACT.h"

static inline FACT_t
FACT_noline_throw (func_t *scope, const char *desc)
{
  FACT_t ret_val;

  ret_val.type = ERROR_TYPE;
  ret_val.error.line = 0;
  ret_val.error.scope = scope;
  ret_val.error.description = (char *) desc;

  return ret_val;
}

#define FACT_ret_error return FACT_noline_throw
#define FACT_throw return FACT_throw_error

FACT_INTERN_FUNC (FACT_t) FACT_throw_error (func_t *, const char *, syn_tree_t);
FACT_INTERN_FUNC (void) errorman_dump (error_t);

#endif
