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

#ifndef COMPARISONS_H_
#define COMPARISONS_H_

#include "FACT.h"

FACT_INTERN_FUNC (FACT_t) equal      (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) not_equal  (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) more       (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) more_equal (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) less       (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) less_equal (FACT_t, FACT_t);

FACT_INTERN_FUNC (FACT_t) and (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) or  (func_t *, syn_tree_t);

#endif
