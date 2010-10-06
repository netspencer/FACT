#include "common.h"

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
mpc_set_str (mpc_t *rop, char *str, unsigned int precision)
{
  /* This function does not take in a string with a decimal point. */
  rop->precision = precision;
  mpz_set_str (rop->object, str, 10);
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
      //mpz_mul (temp.object, temp.object, power_of_ten (op1.precision - op2.precision));
      mpz_add (temp.object, temp.object, op1.object);
    }
  else
    {
      mpz_set (temp.object, op1.object);
      power_of_ten (temp.object, op2.precision - op1.precision);
      //      mpz_mul (temp.object, temp.object, power_of_ten (op2.precision - op1.precision));
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
      //      mpz_mul (temp.object, temp.object, power_of_ten (op1.precision - op2.precision));
      mpz_sub (temp.object, op1.object, temp.object);
    }
  else
    {
      mpz_set (temp.object, op1.object);
      power_of_ten (temp.object, op2.precision - op1.precision);
      //      mpz_mul (temp.object, temp.object, power_of_ten (op2.precision - op1.precision));
      mpz_sub (temp.object, temp.object, op2.object);
    }

  rop->precision = temp.precision;
  mpz_set (rop->object, temp.object);
}

void
mpc_neg (mpc_t *rop, mpc_t op)
{
  rop->precision = op.precision;
  mpz_neg (rop->object, op.object);
}

void
mpc_mul (mpc_t *rop, mpc_t op1, mpc_t op2)
{
  rop->precision = op2.precision + op1.precision;
  mpz_mul (rop->object, op1.object, op2.object);
}

void
mpc_div (mpc_t *rop, mpc_t op1, mpc_t op2)
{
  mpz_t temp;
  /* Division by zero is not checked for in this function. */
  mpz_init (temp);
  
  if (op1.precision == 0)
    {
      rop->precision = default_prec;
      mpz_set (temp, op1.object);
      power_of_ten (temp, rop->precision);
      //      mpz_mul (temp, op1.object, power_of_ten (rop->precision));
    }
  else
    {
      rop->precision = op1.precision;
      mpz_set (temp, op1.object);
    }
  
  mpz_tdiv_q (temp, temp, op2.object);

  mpz_set (rop->object, temp);
}

void
mpc_mod (mpc_t *rop, mpc_t op1, mpc_t op2)
{
  /* Division by zero is not checked, and this function
     literally doesn't even work. It's just filler because
     I'm tired. */
  mpz_mod (rop->object, op1.object, op2.object);
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
      //      mpz_mul (temp, temp, power_of_ten (op1.precision - op2.precision));

      //      gmp_printf ("op1 = %Zd, op2 = %Zd, temp = %Zd\n", op1.object, op2.object, temp);

      return mpz_cmp (op1.object, temp);
    }
  else
    {
      mpz_init_set (temp, op1.object);
      power_of_ten (temp, op2.precision - op1.precision);
      //   mpz_mul (temp, temp, power_of_ten (op2.precision - op1.precision));

      //      gmp_printf ("op1 = %Zd, op2 = %Zd, temp = %Zd\n", op1.object, op2.object, temp);

      return mpz_cmp (temp, op2.object);
    }
}

int
mpc_cmp_ui (mpc_t op1, unsigned long int op2)
{
  unsigned int count;

  for (count = 0; count < op1.precision; count++)
    op2 *= 10;
  //  op2 *= power_of_ten (op1.precision);

  return mpz_cmp_ui (op1.object, op2);
}

int
mpc_cmp_si (mpc_t op1, signed long int op2)
{
  unsigned int count;
  
  for (count = 0; count < op1.precision; count++)
    op2 *= 10;
  //  op2 *= power_of_ten (op1.precision);

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

  /*
  mpz_init_set (temp, rop.object);
  mpz_tdiv_q_ui (temp, temp, power_of_ten (rop.precision));
  */

  return mpz_get_si (temp1);
}

static char *
concatinate_free (char *op1, char *op2, bool fop1, bool fop2)
{
  char *return_value;

  extern char *combine_strs (char *, char *);
  
  return_value = combine_strs (op1, op2);

  if (fop1)
    GC_free (op1);
  if (fop2)
    GC_free (op2);

  return return_value;
}

char *
mpc_get_str (mpc_t op)
{
  char *return_value;
  mpz_t temp_op;
  mpz_t temp1;
  mpz_t temp2;

  /* I'm guessing this will have some issues with negative numbers.
     Will fix that in a bit, if it is the case. */

  extern char *combine_strs (char *, char *);

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

      /*
	Need to fix this up a bit to handle x.0..x;
      */

      return_value = concatinate_free (return_value, mpz_get_str (NULL, 10, temp1), true, true);

      if (mpz_sgn (op.object) < 0)
	return_value = concatinate_free ("-", return_value, false, true);

      mpz_clear (temp1);
      mpz_clear (temp2);
      mpz_clear (temp_op);
    }

  return return_value;
}
