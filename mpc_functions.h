#ifndef MPC_FUNCTIONS_H_
#define MPC_FUNCTIONS_H_

#include "common.h"

/**
 * mpc_functions:
 * This code specifies data structures and
 * functions that allow for arbitrary precision,
 * fast, floating-point arithmetic, which can
 * easily be turned into integer arithmetic.
 * It is all built as an extension of the
 * GMP library.
 *
 * TODO:
 * Improve speed and memory usage, fix mod
 * function so that it actually works.
 *
 * (c) 2010 Matthew Plant, under the GPL.
 */

typedef struct
{
  unsigned int precision; /*
			    Unsigned as there will never be a
			    negative precision. By default the
			    value is 0, indicating 10^0 = 1.
			  */
  mpz_t object;           /*
			    All data is stored in the form of
			    an integer type.
			  */
} mpc_t;

void mpc_set_default_prec (unsigned int);
unsigned int mpc_get_default_prec (void);

void mpc_init (mpc_t *);

void mpc_set (mpc_t *, mpc_t);
void mpc_set_ui (mpc_t *, unsigned long int);
void mpc_set_si (mpc_t *, signed long int);
void mpc_set_str (mpc_t *, char *, unsigned int);

void mpc_add (mpc_t *, mpc_t, mpc_t);
void mpc_sub (mpc_t *, mpc_t, mpc_t);
void mpc_neg (mpc_t *, mpc_t);
void mpc_mul (mpc_t *, mpc_t, mpc_t);
void mpc_div (mpc_t *, mpc_t, mpc_t);
void mpc_mod (mpc_t *, mpc_t, mpc_t);

int mpc_cmp (mpc_t, mpc_t);
int mpc_cmp_ui (mpc_t, unsigned long int);
int mpc_cmp_si (mpc_t, signed long int);

unsigned long int mpc_get_ui (mpc_t);
signed long int mpc_get_si (mpc_t);
char *mpc_get_str (mpc_t);

#endif
