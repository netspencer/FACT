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

#ifndef MALLOC_REPLACEMENTS_H_
#define MALLOC_REPLACEMENTS_H_

#include "FACT.h"

FACT_INTERN_FUNC (void *) FACT_malloc  (size_t);
FACT_INTERN_FUNC (void *) FACT_realloc (void *, size_t);
FACT_INTERN_FUNC (void  ) FACT_free    (void *);

#define better_malloc              FACT_malloc
#define better_realloc(op1, op2)   FACT_realloc (op1, op2)
#define better_free(op1)           FACT_free (op1, 0)

#endif

