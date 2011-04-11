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

#ifndef MPC_FUNCTIONS_H_
#define MPC_FUNCTIONS_H_

#include "FACT.h"

FACT_INTERN_FUNC (void        ) mpc_set_default_prec (unsigned int);
FACT_INTERN_FUNC (unsigned int) mpc_get_default_prec (void        );

FACT_INTERN_FUNC (void) mpc_init    (mpc_t *                           );
FACT_INTERN_FUNC (void) mpc_set     (mpc_t *, mpc_t                    );
FACT_INTERN_FUNC (void) mpc_set_ui  (mpc_t *, unsigned long int        );
FACT_INTERN_FUNC (void) mpc_set_si  (mpc_t *, signed long int          );
FACT_INTERN_FUNC (void) mpc_set_str (mpc_t *, char *, unsigned int, int);

FACT_INTERN_FUNC (void) mpc_add (mpc_t *, mpc_t, mpc_t);
FACT_INTERN_FUNC (void) mpc_sub (mpc_t *, mpc_t, mpc_t);
FACT_INTERN_FUNC (void) mpc_neg (mpc_t *, mpc_t       );
FACT_INTERN_FUNC (void) mpc_mul (mpc_t *, mpc_t, mpc_t);
FACT_INTERN_FUNC (void) mpc_div (mpc_t *, mpc_t, mpc_t);
FACT_INTERN_FUNC (void) mpc_mod (mpc_t *, mpc_t, mpc_t); 
/* Bitwise operations. */
FACT_INTERN_FUNC (void) mpc_and (mpc_t *, mpc_t, mpc_t);
FACT_INTERN_FUNC (void) mpc_ior (mpc_t *, mpc_t, mpc_t);
FACT_INTERN_FUNC (void) mpc_xor (mpc_t *, mpc_t, mpc_t);

FACT_INTERN_FUNC (int) mpc_cmp    (mpc_t, mpc_t            );
FACT_INTERN_FUNC (int) mpc_cmp_ui (mpc_t, unsigned long int);
FACT_INTERN_FUNC (int) mpc_cmp_si (mpc_t, signed long int  );

FACT_INTERN_FUNC (unsigned long int) mpc_get_ui  (mpc_t       );
FACT_INTERN_FUNC (signed long int  ) mpc_get_si  (mpc_t       );
FACT_INTERN_FUNC (char *           ) mpc_get_str (mpc_t       );
FACT_INTERN_FUNC (void             ) mpc_get_mpz (mpz_t, mpc_t);

////////////
// Macros:
////////////

#define mpc_add_ui(o1, o2, o3) mpc_add (o1, o2, FACT_get_ui (o3).v_point->data)
#define mpc_add_si(o1, o2, o3) mpc_add (o1, o2, FACT_get_si (o3).v_point->data)
#define mpc_sub_ui(o1, o2, o3) mpc_sub (o1, o2, FACT_get_ui (o3).v_point->data)
#define mpc_sub_si(o1, o2, o3) mpc_sub (o1, o2, FACT_get_si (o3).v_point->data)
#endif
