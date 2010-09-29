#include "mpc_functions.h"

static unsigned int default_prec = 6;

static inline unsigned int
power_of_ten (unsigned int power)
{
  unsigned int return_value;
  unsigned int counter;

  for (return_value = 1, counter = 0; counter < power; counter++)
    return_value *= 10;

  return return_value;
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
  rop->precision = (op1.precision < op2.precision) ? op2.precision : op1.precision;

  if (op1.precision == op2.precision)
    mpz_add (rop->object, op1.object, op2.object);
  else if (op1.precision > op2.precision)
    {
      mpz_set (rop->object, op2.object);
      mpz_mul_ui (rop->object, rop->object, power_of_ten (op1.precision - op2.precision));
      mpz_add (rop->object, rop->object, op1.object);
    }
  else
    {
      mpz_set (rop->object, op1.object);
      mpz_mul_ui (rop->object, rop->object, power_of_ten (op2.precision - op1.precision));
      mpz_add (rop->object, rop->object, op2.object);
    }
}

void
mpc_sub (mpc_t *rop, mpc_t op1, mpc_t op2)
{
  rop->precision = (op1.precision < op2.precision) ? op2.precision : op1.precision;

  if (op1.precision == op2.precision)
    mpz_sub (rop->object, op1.object, op2.object);
  else if (op1.precision > op2.precision)
    {
      mpz_set (rop->object, op2.object);
      mpz_mul_ui (rop->object, rop->object, power_of_ten (op1.precision - op2.precision));
      mpz_sub (rop->object, rop->object, op1.object);
    }
  else
    {
      mpz_set (rop->object, op1.object);
      mpz_mul_ui (rop->object, rop->object, power_of_ten (op2.precision - op1.precision));
      mpz_sub (rop->object, rop->object, op2.object);
    }
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
  /* Division by zero is not checked for in this function. */
  if (op1.precision == 0)
    rop->precision = default_prec;
  else
    rop->precision = op1.precision;

  mpz_mul_ui (rop->object, op1.object, power_of_ten (rop->precision));
  mpz_tdiv_q (rop->object, rop->object, op2.object);
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
  mpc_t temp;

  if (op1.precision == op2.precision)
    return mpz_cmp (op1.object, op2.object);
  else if (op1.precision > op2.precision)
    {
      mpz_init_set (temp.object, op2.object);
      mpz_mul_ui (temp.object, temp.object, power_of_ten (op1.precision - op2.precision));

      return mpz_cmp (temp.object, op1.object);
    }
  else
    {
      mpz_init_set (temp.object, op1.object);
      mpz_mul_ui (temp.object, temp.object, power_of_ten (op2.precision - op1.precision));

      return mpz_cmp (temp.object, op2.object);
    }
}
  
unsigned long int
mpc_get_ui (mpc_t rop)
{
  mpz_t temp;

  mpz_init_set (temp, rop.object);
  mpz_tdiv_q_ui (temp, power_of_ten (rop.precision));

  return mpz_get_ui (temp);
}

signed long int
mpc_get_si (mpc_t rop)
{
  mpz_t temp;

  mpz_init_set (temp, rop.object);
  mpz_tdiv_q_ui (temp, power_of_ten (rop.precision));

  return mpz_get_si (temp);
}

char *
mpc_get_str (mpc_t op)
{
  unsigned int prec;
  char *return_value;
  char *start;

  return_value = mpz_get_str (NULL, 10, op.object);

  if (op.precision > 0)
    {
      for (start = return_value; return_value[0] != '\0'; return_value++)
	;
      start = (char *) better_realloc (start, (return_value - start + 1) * (sizeof char));

      for (prec = 0; prec < op.precision; prec++)
	return_value[-prec] = return_value[-prec + 1];

      return_value[-prec] = '.';

      return_value = start;
    }

  return return_value;
}
