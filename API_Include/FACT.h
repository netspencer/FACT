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

#ifndef FACT_H_
#define FACT_H_

#define FACT_API_FUNC(type) type
#if (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
# define FACT_API_DEPRECATED(type) __attribute__((__deprecated__)) type
#else
# define FACT_API_DEPRECATED(type) type
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <gmp.h>

#include <FACT/typedefs.h>
#include <FACT/mpc_functions.h>
#include <FACT/utilities.h>
#include <FACT/malloc_replacements.h>
#include <FACT/management.h>
#include <FACT/execfile.h>
/* more to come */

static inline FACT_t
FACT_noline_throw (func_t *scope, const char *desc)
{
  FACT_t ret_val =
    {
      .type = ERROR_TYPE,
      .error =
      {
        .line = 0,
        .scope = scope,
        .description = (char *) desc,
      },
    };
  return ret_val;
}

#define FACT_ret_error return FACT_noline_throw

#endif
