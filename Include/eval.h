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

#ifndef EVAL_H_
#define EVAL_H_

#include "FACT.h"

/* Get or set the instruction pointer. */
FACT_INLINE_FUNC (unsigned long) get_ip    ( void );
FACT_INLINE_FUNC ( void )        set_ip    (unsigned long);
FACT_INLINE_FUNC ( void )        move_ip   (unsigned long);
FACT_INLINE_FUNC ( void )        next_inst ( void );

/* This macro is used to make reseting the ip
 * easier and more logical in certain contexts.
 */
#define reset_ip() set_ip (0)

/* Convert a char ** to a word_list. */
FACT_INTERN_FUNC (syn_tree_t) make_syn_tree (char **, int *);
			     
/* Evaluate and expression or single instruction, or even
 * an entire block of code (procedure/lambda_proc).
 */
FACT_INTERN_FUNC (FACT_t) eval_expression (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) eval            (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) procedure       (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) lambda_proc     (func_t *, syn_tree_t);

#endif
