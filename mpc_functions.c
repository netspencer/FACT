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

#include "FACT.h"

static unsigned int default_prec = 6;

void
power_of_ten (mpz_t rop, unsigned int op)
{
  unsigned int counter;

  for (counter = 0; counter < op; counter++)
    mpz_mul_ui (rop, rop, 10);
}

void
mpc_set_default_prec (unsigned int prec)
{
  default_prec = prec;
}

unsigned int
mpc_get_default_prec ()
{
  return default_prec;
}

void
mpc_init (mpc_t *CONCAT)
{
  CONCAT->precision = 0;
  mpz_init (CONCAT->object);
}

void
mpc_set (mpc_t *rop, mpc_t op)
{
  rop->precision = op.precision;
  mpz_set (rop->object, op.object);
}

void
mpc_set_ui (mpc_t *rop, unsigned long int op)
{
  rop->precision = 0;
  mpz_set_ui (rop->object, op);
}

void
mpc_set_si (mpc_t *rop, signed long int op)
{
  rop->precision = 0;
  mpz_set_si (rop->object, op);
}

void
mpc_set_str (mpc_t *rop, char *str, unsigned int precision, int base)
{
  /* This function does not take in a string with a decimal point. */
  rop->precision = precision;
  mpz_set_str (rop->object, str, base);
}

void
mpc_add (mpc_t *rop, mpc_t op1, mpc_t op2)
{
  mpc_t temp;
  
  temp.precision = (op1.precision < op2.precision) ? op2.precision : op1.precision;
  mpz_init (temp.object);

  if (op1.precision == op2.precision)
    mpz_add (temp.object, op1.object, op2.object);
  else if (op1.precision > op2.precision)
    {
      mpz_set (temp.object, op2.object);
      power_of_ten (temp.object, op1.precision - op2.precision);
      mpz_add (temp.object, temp.object, op1.object);
    }
  else
    {
      mpz_set (temp.object, op1.object);
      power_of_ten (temp.object, op2.precision - op1.precision);
      mpz_add (temp.object, temp.object, op2.object);
    }

  rop->precision = temp.precision;
  mpz_set (rop->object, temp.object);
}

void
mpc_sub (mpc_t *rop, mpc_t op1, mpc_t op2)
{
  mpc_t temp;
  
  temp.precision = (op1.precision < op2.precision) ? op2.precision : op1.precision;
  mpz_init (temp.object);

  if (op1.precision == op2.precision)
    mpz_sub (temp.object, op1.object, op2.object);
  else if (op1.precision > op2.precision)
    {
      mpz_set (temp.object, op2.object);
      power_of_ten (temp.object, op1.precision - op2.precision);
      mpz_sub (temp.object, op1.object, temp.object);
    }
  else
    {
      mpz_set (temp.object, op1.object);
      power_of_ten (temp.object, op2.precision - op1.precision);
      mpz_sub (temp.object, temp.object, op2.object);
    }

  rop->precision = temp.precision;
  mpz_set (rop->object, temp.object);
}

void
mpc_neg (mpc_t *rop, mpc_t op)
{
  mpc_t temp;

  temp.precision = op.precision;
  mpz_init (temp.object);
  mpz_set (temp.object, op.object);
  
  rop->precision = op.precision;
  mpz_neg (rop->object, temp.object);
}

void
mpc_mul (mpc_t *rop, mpc_t op1, mpc_t op2)
{
  mpz_t temp;

  mpz_init (temp);
  mpz_mul (temp, op1.object, op2.object);
  mpz_set (rop->object, temp);

  rop->precision = op2.precision + op1.precision;
}

void
mpc_div (mpc_t *rop, mpc_t op1, mpc_t op2) 
{
  /* This function needs to be fixed. At the current moment I am tired
   * of having to mess around with the precision values, so I'm just
   * going to convert the values to 'mpf's and call it quits for the
   * day. I'll fix it later.
   * Division by zero is not checked for in this function.
   */
  mpf_t hold_op1;
  mpf_t hold_op2;
  mpf_t hold_res;
  unsigned int prec;

  mpf_init (hold_op1);
  mpf_init (hold_op2);
  mpf_init (hold_res);
  mpf_set_z (hold_op1, op1.object);
  mpf_set_z (hold_op2, op2.object);

  // Get the largest precision
  prec = (op1.precision > op2.precision) ? op1.precision : op2.precision;
  if (prec == 0)
    prec = default_prec;

  // Set the scalar values
  while (op1.precision-- > 0)
    mpf_div_ui (hold_op1, hold_op1, 10);
  while (op2.precision-- > 0)
    mpf_div_ui (hold_op2, hold_op2, 10);

  // Get the value
  mpf_div (hold_res, hold_op1, hold_op2);

  for (rop->precision = prec; prec > 0; prec--)
    mpf_mul_ui (hold_res, hold_res, 10);
  
  mpz_set_f (rop->object, hold_res);
  /*
  temp.precision = (op1.precision < op2.precision) ? op2.precision : op1.precision;

  if (!op1.precision && !op2.precision)
    {
      temp.precision = default_prec;
      mpz_set (temp.object, op1.object);
      power_of_ten (temp.object, default_prec);
      mpz_tdiv_q (temp_res, temp.object, op2.object);
    }
  else if (op1.precision < op2.precision)
    {
      temp.precision = op2.precision;
      mpz_set (temp.object, op1.object);
      power_of_ten (temp.object, op2.precision - op1.precision);
      mpz_tdiv_q (temp_res, temp.object, op2.object);
    }
  else if (op1.precision > op2.precision)
    {
      temp.precision = op1.precision;
      mpz_set (temp.object, op2.object);
      power_of_ten (temp.object, op1.precision - op2.precision);
      mpz_tdiv_q (temp_res, op1.object, temp.object);
    }
  else
    {
      temp.precision = op1.precision;
      mpz_set (temp.object, op1.object);
      mpz_tdiv_q (temp_res, temp.object, op2.object);
    }

  rop->precision = temp.precision;
  mpz_set (rop->object, temp_res);
  */
}

void
mpc_mod (mpc_t *rop, mpc_t op1, mpc_t op2)
{
  /* I am 90% sure that this doesn't work. However, it works for
   * integers, so I'll put off fixing it for a little while.
   */
  mpf_t hold_op1;
  mpf_t hold_op2;
  mpf_t hold_res;
  unsigned int prec;

  mpf_init (hold_op1);
  mpf_init (hold_op2);
  mpf_init (hold_res);
  mpf_set_z (hold_op1, op1.object);
  mpf_set_z (hold_op2, op2.object);

  // Get the largest precision.
  prec = (op1.precision > op2.precision) ? op1.precision : op2.precision;

  // Set the scalar values
  while (op1.precision-- > 0)
    mpf_div_ui (hold_op1, hold_op1, 10);
  while (op2.precision-- > 0)
    mpf_div_ui (hold_op2, hold_op2, 10);

  // Get the value
  mpf_div (hold_res, hold_op1, hold_op2);
  mpf_floor (hold_res, hold_res);
  mpf_mul (hold_res, hold_res, hold_op2);
  mpf_sub (hold_res, hold_op1, hold_res);

  for (rop->precision = prec; prec > 0; prec--)
    mpf_mul_ui (hold_res, hold_res, 10);
  
  mpz_set_f (rop->object, hold_res);
}

void
mpc_and (mpc_t *rop, mpc_t op1, mpc_t op2)
{
  /* Little did I realize that I would eventually make it
   * an error to apply bitwise operators to floating point
   * variables.
   */
  mpc_t temp;
  
  temp.precision = (op1.precision < op2.precision) ? op2.precision : op1.precision;
  mpz_init (temp.object);

  if (op1.precision == op2.precision)
    mpz_and (temp.object, op1.object, op2.object);
  else if (op1.precision > op2.precision)
    {
      mpz_set (temp.object, op2.object);
      power_of_ten (temp.object, op1.precision - op2.precision);
      mpz_and (temp.object, op1.object, temp.object);
    }
  else
    {
      mpz_set (temp.object, op1.object);
      power_of_ten (temp.object, op2.precision - op1.precision);
      mpz_and (temp.object, temp.object, op2.object);
    }

  rop->precision = temp.precision;
  mpz_set (rop->object, temp.object);
}

void
mpc_ior (mpc_t *rop, mpc_t op1, mpc_t op2)
{
  mpc_t temp;
  
  temp.precision = (op1.precision < op2.precision) ? op2.precision : op1.precision;
  mpz_init (temp.object);

  if (op1.precision == op2.precision)
    mpz_ior (temp.object, op1.object, op2.object);
  else if (op1.precision > op2.precision)
    {
      mpz_set (temp.object, op2.object);
      power_of_ten (temp.object, op1.precision - op2.precision);
      mpz_ior (temp.object, op1.object, temp.object);
    }
  else
    {
      mpz_set (temp.object, op1.object);
      power_of_ten (temp.object, op2.precision - op1.precision);
      mpz_ior (temp.object, temp.object, op2.object);
    }

  rop->precision = temp.precision;
  mpz_set (rop->object, temp.object);
}

void
mpc_xor (mpc_t *rop, mpc_t op1, mpc_t op2)
{
  mpc_t temp;
  
  temp.precision = (op1.precision < op2.precision) ? op2.precision : op1.precision;
  mpz_init (temp.object);

  if (op1.precision == op2.precision)
    mpz_xor (temp.object, op1.object, op2.object);
  else if (op1.precision > op2.precision)
    {
      mpz_set (temp.object, op2.object);
      power_of_ten (temp.object, op1.precision - op2.precision);
      mpz_xor (temp.object, op1.object, temp.object);
    }
  else
    {
      mpz_set (temp.object, op1.object);
      power_of_ten (temp.object, op2.precision - op1.precision);
      mpz_xor (temp.object, temp.object, op2.object);
    }

  rop->precision = temp.precision;
  mpz_set (rop->object, temp.object);
}

int
mpc_cmp (mpc_t op1, mpc_t op2)
{
  mpz_t temp;

  if (op1.precision == op2.precision)
    return mpz_cmp (op1.object, op2.object);
  else if (op1.precision > op2.precision)
    {
      mpz_init_set (temp, op2.object);
      power_of_ten (temp, op1.precision - op2.precision);

      return mpz_cmp (op1.object, temp);
    }
  else
    {
      mpz_init_set (temp, op1.object);
      power_of_ten (temp, op2.precision - op1.precision);

      return mpz_cmp (temp, op2.object);
    }
}

int
mpc_cmp_ui (mpc_t op1, unsigned long int op2)
{
  unsigned int count;

  for (count = 0; count < op1.precision; count++)
    op2 *= 10;

  return mpz_cmp_ui (op1.object, op2);
}

int
mpc_cmp_si (mpc_t op1, signed long int op2)
{
  unsigned int count;
  
  for (count = 0; count < op1.precision; count++)
    op2 *= 10;

  return mpz_cmp_si (op1.object, op2);
}

unsigned long int
mpc_get_ui (mpc_t rop)
{
  mpz_t temp1;
  mpz_t temp2;

  mpz_init_set (temp1, rop.object);
  mpz_init_set_ui (temp2, 1);
  power_of_ten (temp2, rop.precision);
  mpz_tdiv_q (temp1, temp1, temp2);

  return mpz_get_ui (temp1);
}

signed long int
mpc_get_si (mpc_t rop)
{
  mpz_t temp1;
  mpz_t temp2;

  mpz_init_set (temp1, rop.object);
  mpz_init_set_ui (temp2, 1);
  power_of_ten (temp2, rop.precision);
  mpz_tdiv_q (temp1, temp1, temp2);

  return mpz_get_si (temp1);
}

static char *
concatinate_free (char *op1, char *op2, bool fop1, bool fop2)
{
  char *return_value;

  return_value = combine_strs (op1, op2);

  if (fop1)
    FACT_free (op1);
  if (fop2)
    FACT_free (op2);

  return return_value;
}

char *
mpc_get_str (mpc_t op)
{
  char *return_value;
  mpz_t temp_op;
  mpz_t temp1;
  mpz_t temp2;

  if (op.precision == 0)
    return_value = mpz_get_str (NULL, 10, op.object);
  else
    {
      mpz_init (temp_op);
      mpz_abs (temp_op, op.object);
      
      mpz_init_set (temp1, temp_op);
      mpz_init_set_ui (temp2, 1);

      power_of_ten (temp2, op.precision);
      mpz_tdiv_q (temp1, temp1, temp2);

      return_value = concatinate_free (mpz_get_str (NULL, 10, temp1), ".", true, false);

      mpz_mul (temp1, temp1, temp2);
      mpz_sub (temp1, temp_op, temp1);

      mpz_abs (temp1, temp1);

      mpz_tdiv_q_ui (temp2, temp2, 10);

      while (mpz_cmp (temp2, temp1) > 0)
	{
	  return_value = concatinate_free (return_value, "0", true, false);
	  mpz_tdiv_q_ui (temp2, temp2, 10);
	}

      return_value = concatinate_free (return_value, mpz_get_str (NULL, 10, temp1), true, true);

      if (mpz_sgn (op.object) < 0)
	return_value = concatinate_free ("-", return_value, false, true);

      mpz_clear (temp1);
      mpz_clear (temp2);
      mpz_clear (temp_op);
    }

  return return_value;
}

void
mpc_get_mpz (mpz_t rop, mpc_t op)
{
  mpz_set (rop, op.object);
  while (op.precision-- != 0)
    mpz_tdiv_q_ui (rop, rop, 10);
}
