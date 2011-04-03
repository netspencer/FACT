#include "FACT.h"

FACT_t
equal (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();
  
  if (arg1.type == VAR_TYPE)
    {
      if (compare_var_arrays (arg1.v_point, arg2.v_point, true))
        mpc_set_si (&(return_value.v_point->data), 1);
    }
  else
    {
      // Not good enough.
      if (arg1.f_point->vars == arg2.f_point->vars
	  && arg1.f_point->funcs == arg2.f_point->funcs)
        mpc_set_si (&(return_value.v_point->data), 1);
    }

  return return_value;
}

FACT_t
not_equal (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();
    
  if (arg1.type == VAR_TYPE)
    {
      if (!compare_var_arrays (arg1.v_point, arg2.v_point, true))
        mpc_set_si (&(return_value.v_point->data), 1);
    }
  else
    {
      if (arg1.f_point->vars != arg2.f_point->vars
          || arg1.f_point->funcs != arg2.f_point->funcs)
        mpc_set_si (&(return_value.v_point->data), 1);
    }

  return return_value;
}

FACT_t
more (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (mpc_cmp (arg1.v_point->data, arg2.v_point->data) > 0)
    mpc_set_si (&(return_value.v_point->data), 1);

  return return_value;
}

FACT_t
more_equal (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (mpc_cmp (arg1.v_point->data, arg2.v_point->data) >= 0)
    mpc_set_si (&(return_value.v_point->data), 1);

  return return_value;
}

FACT_t
less (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;
  
  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (mpc_cmp (arg1.v_point->data, arg2.v_point->data) < 0)
    mpc_set_si (&(return_value.v_point->data), 1);

  return return_value;
}

FACT_t
less_equal (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (mpc_cmp (arg1.v_point->data, arg2.v_point->data) <= 0)
    mpc_set_si (&(return_value.v_point->data), 1);

  return return_value;
}

static int
statement_length (char **words)
{
  int i;
  int w_code;
  int arguments_left;
  char *token;

  arguments_left = 1;
  
  for (i = 0; arguments_left > 0 && words[i] != NULL; i++, arguments_left--)
    {
      token  = get_bcode_label (words[i]);
      w_code = op_get_prec ((token != NULL) ? token : words[i]);
      
      switch (w_code)
	{
	case 18:
	  return i; 

        case 17:
          if (!strcmp (words[i], "[") || !strcmp (words[i], "!["))
            {
              i += get_exp_length  (words + i + 1, ']');
              token = get_bcode_label (words[i + 1]);
              if (words[i + 1][0] != BYTECODE && !strcmp (words[i + 1], "["))
                arguments_left++;
            }
          else if (!strcmp (words[i], "{"))
            i += get_exp_length (words + i + 1, '}');
          else if (!strcmp (words[i], "("))
            i += get_exp_length (words + i + 1, ')');
          break;

        case 14:
        case 13:
        case 12:
        case 11:
        case 10:
        case 9:
        case 8:
        case 7:
        case 6:
        case 5:
        case 4:
        case 3:
	  arguments_left += 2;
	  break;
          
        case 2:
        case 1:
          arguments_left++;
	  break;

        case 0:
	  for (;;)
            {
              if (words[++i] != NULL && words[i][0] != BYTECODE
                  && op_get_prec (words[i]) == -1)
                {
                  i--;
                  break;
                }
	    }
	  break;

	case -1:
	  if (words[i + 1] != NULL && words[i + 1][0] != BYTECODE
              && !strcmp (words[i + 1], "["))
	    arguments_left++;
	  break;

	default:
	  break;
	}
    }

  return i;
}

FACT_t
and (func_t *scope, syn_tree_t expression)
{
  int           len;
  FACT_t        arg1;
  FACT_t        arg2;
  FACT_t        return_value;
  unsigned long ip;

  ip = get_ip ();
  reset_ip ();
  expression.syntax += ip;

  if ((arg1 = eval (scope, expression)).type == ERROR_TYPE)
    return arg1;
  else if (arg1.type == FUNCTION_TYPE)
    FACT_throw (scope, "both arguments to && must be variables", expression);

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();
  expression.syntax += get_ip ();
  len = statement_length (expression.syntax);

  if (!mpc_cmp_si (arg1.v_point->data, 0))
    {
      move_ip (ip + len);
      return return_value;
    }
  
  ip += get_ip ();
  reset_ip ();

  if ((arg2 = eval (scope, expression)).type == ERROR_TYPE)
    return arg2;
  else if (arg2.type == FUNCTION_TYPE)
    FACT_throw (scope, "both arguments to && must be variables", expression);

  if (mpc_cmp_si (arg2.v_point->data, 0) == 0)
    return return_value;

  mpc_set_si (&(return_value.v_point->data), 1);
  move_ip (ip);

  return return_value;
}

FACT_t
or (func_t *scope, syn_tree_t expression)
{
  int           len;
  FACT_t        arg1;
  FACT_t        arg2;
  FACT_t        return_value;
  unsigned long ip;

  ip = get_ip ();
  reset_ip ();
  expression.syntax += ip;
  
  if ((arg1 = eval (scope, expression)).type == ERROR_TYPE)
    return arg1;
  else if (arg1.type == FUNCTION_TYPE)
    FACT_throw (scope, "both arguments to || must be variables", expression);

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();
  mpc_set_ui (&(return_value.v_point->data), 1);
  expression.syntax += get_ip ();
  len = statement_length (expression.syntax);

  if (mpc_cmp_si (arg1.v_point->data, 0))
    {
      move_ip (ip + len);
      return return_value;
    }

  ip += get_ip ();
  reset_ip ();

  if ((arg2 = eval (scope, expression)).type == ERROR_TYPE)
    return arg2;
  else if (arg2.type == FUNCTION_TYPE)
    FACT_throw (scope, "both arguments to || must be variables", expression);

  if (mpc_cmp_si (arg2.v_point->data, 0))
    return return_value;

  mpc_set_ui (&(return_value.v_point->data), 0);
  move_ip (ip);
  return return_value;
}
