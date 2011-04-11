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

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include "FACT.h"

FACT_INTERN_FUNC (FACT_t) liven_func (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) run_func   (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) new_scope  (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) in_scope   (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) lambda     (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) up         (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) this       (func_t *, syn_tree_t); 
FACT_INTERN_FUNC (FACT_t) NULL_func  (func_t *, syn_tree_t);

#endif
