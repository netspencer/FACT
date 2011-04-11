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

#ifndef MATH_H_
#define MATH_H_

#include "FACT.h"

FACT_INTERN_FUNC (bool  ) isnum      (char *);
FACT_INTERN_FUNC (FACT_t) num_to_var (char *);      /* converts a string to a var_t. */

FACT_INTERN_FUNC (FACT_t) add    (FACT_t, FACT_t);  /* adds two numbers together     */
FACT_INTERN_FUNC (FACT_t) sub    (FACT_t, FACT_t);  /* subtracts two numbers         */
FACT_INTERN_FUNC (FACT_t) mult   (FACT_t, FACT_t);  /* multiplies two numbers        */
FACT_INTERN_FUNC (FACT_t) divide (FACT_t, FACT_t);  /* divides two numbers           */
FACT_INTERN_FUNC (FACT_t) mod    (FACT_t, FACT_t);  /* modulos two numbers           */
FACT_INTERN_FUNC (FACT_t) bit_and (FACT_t, FACT_t); 
FACT_INTERN_FUNC (FACT_t) bit_ior (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) bit_xor (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) bit_not (func_t *, syn_tree_t);

FACT_INTERN_FUNC (FACT_t) add_assignment  (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) sub_assignment  (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) mult_assignment (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) div_assignment  (FACT_t, FACT_t);
FACT_INTERN_FUNC (FACT_t) mod_assignment  (FACT_t, FACT_t);

FACT_INTERN_FUNC (FACT_t) negative  (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) paren     (func_t *, syn_tree_t);

#endif
