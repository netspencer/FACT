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

#ifdef __cplusplus
extern "C" { 
#endif

FACT_API_FUNC (void) mpc_set_default_prec (unsigned int);
FACT_API_FUNC (unsigned int) mpc_get_default_prec (void);

FACT_API_FUNC (void) mpc_init (mpc_t *);

FACT_API_FUNC (void) mpc_set     (mpc_t *, mpc_t);
FACT_API_FUNC (void) mpc_set_ui  (mpc_t *, unsigned long int);
FACT_API_FUNC (void) mpc_set_si  (mpc_t *, signed long int);
FACT_API_FUNC (void) mpc_set_str (mpc_t *, char *, unsigned int);

FACT_API_FUNC (void) mpc_add (mpc_t *, mpc_t, mpc_t);
FACT_API_FUNC (void) mpc_sub (mpc_t *, mpc_t, mpc_t);
FACT_API_FUNC (void) mpc_neg (mpc_t *, mpc_t);
FACT_API_FUNC (void) mpc_mul (mpc_t *, mpc_t, mpc_t);
FACT_API_FUNC (void) mpc_div (mpc_t *, mpc_t, mpc_t);
FACT_API_FUNC (void) mpc_mod (mpc_t *, mpc_t, mpc_t);

FACT_API_FUNC (void) mpc_and (mpc_t *, mpc_t, mpc_t);
FACT_API_FUNC (void) mpc_ior (mpc_t *, mpc_t, mpc_t);
FACT_API_FUNC (void) mpc_xor (mpc_t *, mpc_t, mpc_t);

FACT_API_FUNC (int) mpc_cmp    (mpc_t, mpc_t);
FACT_API_FUNC (int) mpc_cmp_ui (mpc_t, unsigned long int);
FACT_API_FUNC (int) mpc_cmp_si (mpc_t, signed long int);

FACT_API_FUNC (unsigned long int) mpc_get_ui  (mpc_t       );
FACT_API_FUNC (signed long int  ) mpc_get_si  (mpc_t       );
FACT_API_FUNC (char *           ) mpc_get_str (mpc_t       );
FACT_API_FUNC (void             ) mpc_get_mpz (mpz_t, mpc_t);

#ifdef __cplusplus
}
#endif

/* macros to make everything better. */

#define mpc_init(op) mpc_init(&op)

#define mpc_set(op1, op2) mpc_set (&op1, op2)
#define mpc_set_ui(op1, op2) mpc_set_ui (&op1, op2)
#define mpc_set_si(op1, op2) mpc_set_si (&op1, op2)
#define mpc_set_str(op1, op2, op3) mpc_set_str (&op1, op2)

#define mpc_add(op1, op2, op3) mpc_add (&op1, op2, op3)
#define mpc_sub(op1, op2, op3) mpc_sub (&op1, op2, op3)
#define mpc_neg(op1, op2, op3) mpc_neg (&op1, op2, op3)
#define mpc_mul(op1, op2, op3) mpc_mul (&op1, op2, op3)
#define mpc_div(op1, op2, op3) mpc_div (&op1, op2, op3)
#define mpc_mod(op1, op2, op3) mpc_mod (&op1, op2, op3)

/* These macros give the "real" function, without the wrapper */

#define mpc_init_real mpc_init

#define mpc_set_real mpc_set
#define mpc_set_ui_real mpc_set_ui
#define mpc_set_si_real mpc_set_si
#define mpc_set_str_real mpc_set_str

#define mpc_add_real mpc_add
#define mpc_sub_real mpc_sub
#define mpc_neg_real mpc_neg
#define mpc_mul_real mpc_mul
#define mpc_div_real mpc_div
#define mpc_mod_real mpc_mod

#endif
