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

#ifndef MEM_H_
#define MEM_H_

#include "FACT.h"

FACT_INTERN_FUNC (FACT_t) get_array_size  (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) define          (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) defunc          (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) set             (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) return_array    (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) size_of         (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) get_array_func  (func_t *, func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) get_array_var   (var_t  *, func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) combine_arrays  (FACT_t, FACT_t);

FACT_INTERN_FUNC (var_t *) clone_var (var_t *, char *);
FACT_INTERN_FUNC (var_t *) clone_var_f (var_t *, char *);

#endif
