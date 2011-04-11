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

#ifndef STATEMENTS_H_
#define STATEMENTS_H_

#include "FACT.h"

FACT_INTERN_FUNC (FACT_t) if_statement (func_t *, syn_tree_t, bool *);
FACT_INTERN_FUNC (FACT_t) error        (func_t *, syn_tree_t, bool *);
FACT_INTERN_FUNC (FACT_t) else_clause  (func_t *, syn_tree_t        );
FACT_INTERN_FUNC (FACT_t) while_loop   (func_t *, syn_tree_t        );
FACT_INTERN_FUNC (FACT_t) for_loop     (func_t *, syn_tree_t        );
 
#endif
