#include "FACT.h"

bool
isnum (char *word)
{
  bool         hex;
  bool         flp;
  unsigned int i;

  if (word[0] == '0' && tolower (word[1]) == 'x')
    hex = true;
  else
    hex = false;
  
  for (i = (hex) ? 2 : 0, flp = false; word[i] != '\0'; i++)
    {
      if (word[i] == '.')
	{
	  if (flp || word[i + 1] == '\0')
	    return false;
	  flp = true;
	}
      else if (!isdigit ((int) word[i]))
        {
          if (!hex || tolower (word[i]) < 'a' || tolower (word[i]) > 'f')
            return false;
        }
    }

  return true;
}

unsigned int
get_prec (char *word)
{
  bool         is_decimal;
  unsigned int i;
  unsigned int prec;

  for (i = prec = 0, is_decimal = false; word[i] != '\0'; i++)
    {
      if (is_decimal)
	prec++;

      if (word[i] == '.')
	{
	  is_decimal = true;
	  word[i] = ' ';
	}
    }

  return prec;
}

FACT_t
num_to_var (char *word)
{
  FACT_t ret_val;
  
  ret_val.type    = VAR_TYPE;
  ret_val.v_point = alloc_var ();

  if (word[0] == '0' && tolower (word[1]) == 'x')
    mpc_set_str (&(ret_val.v_point->data), word + 2, get_prec (word), 16);
  else
    mpc_set_str (&(ret_val.v_point->data), word, get_prec (word), 10);

  mpz_set_ui (ret_val.v_point->array_size, 1);
  ret_val.v_point->array_up = NULL;
  ret_val.v_point->next     = NULL;
  
  return ret_val;
}

FACT_t
add (FACT_t arg1, FACT_t arg2)
{
  FACT_t ret_val;
  
  ret_val.type    = VAR_TYPE;
  ret_val.v_point = alloc_var ();
  mpc_add (&(ret_val.v_point->data), arg1.v_point->data, arg2.v_point->data);
  
  return ret_val;
}

FACT_t
sub (FACT_t arg1, FACT_t arg2)
{
  FACT_t ret_val;

  ret_val.type    = VAR_TYPE;  
  ret_val.v_point = alloc_var ();

  mpc_sub (&(ret_val.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return ret_val;
}

FACT_t
mult (FACT_t arg1, FACT_t arg2)
{
  FACT_t ret_val;

  ret_val.type    = VAR_TYPE;
  ret_val.v_point = alloc_var ();
  mpc_mul (&(ret_val.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return ret_val;
}

FACT_t
divide (FACT_t arg1, FACT_t arg2)
{
  FACT_t ret_val;

  // Divide by zero not checked for.
  ret_val.type    = VAR_TYPE;
  ret_val.v_point = alloc_var ();
  mpc_div (&(ret_val.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return ret_val;
}

FACT_t
mod (FACT_t arg1, FACT_t arg2)
{
  FACT_t ret_val;

  // Mod by zero not checked.
  ret_val.type    = VAR_TYPE;
  ret_val.v_point = alloc_var ();
  mpc_mod (&(ret_val.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return ret_val;
}

FACT_t
bit_and (FACT_t arg1, FACT_t arg2)
{
  FACT_t ret_val;

  ret_val.type    = VAR_TYPE;
  ret_val.v_point = alloc_var ();
  mpc_and (&(ret_val.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return ret_val;
}

FACT_t
bit_ior (FACT_t arg1, FACT_t arg2)
{
  FACT_t ret_val;

  ret_val.type    = VAR_TYPE;
  ret_val.v_point = alloc_var ();
  mpc_ior (&(ret_val.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return ret_val;
}

FACT_t
bit_xor (FACT_t arg1, FACT_t arg2)
{
  FACT_t ret_val;

  ret_val.type    = VAR_TYPE;
  ret_val.v_point = alloc_var ();
  mpc_xor (&(ret_val.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return ret_val;
}

FACT_t
add_assignment (FACT_t arg1, FACT_t arg2)
{
  if (arg1.v_point->locked)
    return add (arg1, arg2);

  mpc_add (&(arg1.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return arg1;
}

FACT_t
sub_assignment (FACT_t arg1, FACT_t arg2)
{
  if (arg1.v_point->locked)
    return sub (arg1, arg2);
  
  mpc_sub (&(arg1.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return arg1;
}

FACT_t
mult_assignment (FACT_t arg1, FACT_t arg2)
{
  if (arg1.v_point->locked)
    return mult (arg1, arg2);

  mpc_mul (&(arg1.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return arg1;
}

FACT_t
div_assignment (FACT_t arg1, FACT_t arg2)
{
  // Divide by zero not checked for.
  if (arg1.v_point->locked)
    return divide (arg1, arg2);

  mpc_div (&(arg1.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return arg1;
}

FACT_t
mod_assignment (FACT_t arg1, FACT_t arg2)
{
  // Mod by zero not checked.
  if (arg1.v_point->locked)
    return mod (arg1, arg2);

  mpc_mod (&(arg1.v_point->data), arg1.v_point->data, arg2.v_point->data);

  return arg1;

}

FACT_t
negative (func_t *scope, syn_tree_t exp)
{
  FACT_t op;
  FACT_t ret_val;
  
  op = eval (scope, exp);

  if (op.type == ERROR_TYPE)
    return op;
  if (op.type == FUNCTION_TYPE)
    FACT_throw (NULL, "cannot make a function negative", exp);

  ret_val.type    = VAR_TYPE;
  ret_val.v_point = alloc_var ();
  mpc_neg (&(ret_val.v_point->data), op.v_point->data);

  return ret_val;
}

FACT_t
paren (func_t *scope, syn_tree_t exp)
{
  int    len;
  FACT_t return_value;
  unsigned long ip;

  exp.syntax += get_ip ();

  ip = get_ip ();
  reset_ip ();

  len = get_exp_length (exp.syntax, ')');

  /*
  if (index == 0)
    return errorman_throw_reg (scope, "parenthesis has no body");
   
  expression.syntax[index - 1] = NULL;
  */
  return_value = eval (scope, exp);
  // expression.syntax[index - 1] = ")";

  set_ip (ip + len);

  return return_value;
}
