#include "common.h"

/*---------------------------------------------*
 * operators.c: Provides functions for         *
 * comparring data and logical operators.      *
 *                                             *
 * This code is (C) 2010 Matthew Plant.        *
 *---------------------------------------------*/

/*---------------------------------------------*
 * Functions: The actual code. All of these    *
 * functions will return 1 if the arguments    *
 * passed to it meet their conditions, and 0   *
 * if they do not.                             *
 *---------------------------------------------*/

/* * * * * * * * * * * * * * * * * * * * * * * *
 * equal: checks if two arguments are equal to *
 * each other.                                 *
 * * * * * * * * * * * * * * * * * * * * * * * */

static int
check_dimensions (var_t *op1, var_t *op2)
{
  /* Return 0 on failure and 1 on success. */
  if ((op1 == NULL && op2 != NULL)
      || (op1 != NULL && op2 == NULL))
    return 0;
  else if (op1 == NULL && op2 == NULL)
    return 1;
  else if (mpc_cmp (op1->data, op2->data) == 0)
    {
      return (check_dimensions (op1->array_up, op2->array_up)
	      * check_dimensions (op1->next, op2->next));
    }
  else
    return 0;
}

FACT_t
equal (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();
    
  if (arg1.type != arg2.type)
    return errorman_throw_reg (NULL, "argument types to == do not match");
  
  if (arg1.type == VAR_TYPE)
    {
      if (mpc_cmp (arg1.v_point->data, arg2.v_point->data) == 0
	  && check_dimensions (arg1.v_point->array_up, arg2.v_point->array_up))
        mpc_set_si (&(return_value.v_point->data), 1);
    }
  else
    {
      /* Eh, best I can do. */
      if (arg1.f_point->vars == arg2.f_point->vars
	  && arg1.f_point->funcs == arg2.f_point->funcs)
        mpc_set_si (&(return_value.v_point->data), 1);
    }

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * *
 * not_equal: checks if two arguments are    *
 * not equal to each other.                  *
 * * * * * * * * * * * * * * * * * * * * * * */

FACT_t
not_equal (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (arg1.type != arg2.type)
    return errorman_throw_reg (NULL, "argument types to != do not match");
    
  if (arg1.type == VAR_TYPE)
    {
      if (mpc_cmp (arg1.v_point->data, arg2.v_point->data) != 0)
        mpc_set_si (&(return_value.v_point->data), 1);
    }
  else
    {
      if (arg1.f_point != arg2.f_point)
        mpc_set_si (&(return_value.v_point->data), 1);
    }

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * *
 * more: checks if the first argument is     *
 * greater in value than the other arguemnt. *
 * If either argument is not a var_tiable, it  *
 * throws an error.                          *
 * * * * * * * * * * * * * * * * * * * * * * */

FACT_t
more (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (NULL, "arguments to > must be vars");

  if (mpc_cmp (arg1.v_point->data, arg2.v_point->data) > 0)
    mpc_set_si (&(return_value.v_point->data), 1);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * *
 * more_equal: checks if the first argument  *
 * is greater or equal in value to the other *
 * arguemnt. If either argument is not a     *
 * var_tiable, it throws an error.             *
 * * * * * * * * * * * * * * * * * * * * * * */

FACT_t
more_equal (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();
  
  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (NULL, "arguments to >= must be vars");
  
  if (mpc_cmp (arg1.v_point->data, arg2.v_point->data) >= 0)
    mpc_set_si (&(return_value.v_point->data), 1);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * *
 * less: checks if the first argument is     *
 * smaller in value than the other arguemnt. *
 * If either argument is not a variable, it  *
 * throws an error.                          *
 * * * * * * * * * * * * * * * * * * * * * * */

FACT_t
less (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;
  
  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (NULL, "arguments to < must be variables");

  if (mpc_cmp (arg1.v_point->data, arg2.v_point->data) < 0)
    mpc_set_si (&(return_value.v_point->data), 1);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * *
 * less_equal: checks if the first argument  *
 * is smaller or equal in value to the other *
 * arguemnt. If either argument is not a     *
 * variable, it throws an error.             *
 * * * * * * * * * * * * * * * * * * * * * * */

FACT_t
less_equal (FACT_t arg1, FACT_t arg2)
{
  FACT_t return_value;

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (NULL, "arguments to <= must be variables");

  if (mpc_cmp (arg1.v_point->data, arg2.v_point->data) <= 0)
    mpc_set_si (&(return_value.v_point->data), 1);

  return return_value;
}

static int
statement_length (char **words)
{
  int    index          ;
  int    w_code         ;
  int    arguments_left ;
  char * token          ;

  arguments_left = 1;
  
  for (index = 0; arguments_left > 0 && words[index] != NULL; index++, arguments_left--)
    {
      token  = get_bcode_label (words[index]);
      w_code = (token != NULL) ? get_block_code (token) : get_block_code (words[index]);
      
      switch (w_code)
	{
	case CL_PAREN:
	case CL_BRACKET:
	case CL_CURLY:
	  return index; 

	case OP_BRACKET:
	case NOP_BRACKET:
	  index += get_exp_length  (words + index + 1, ']');
	  token  = get_bcode_label (words[index + 1]);
	  w_code = (token != NULL) ? get_block_code (token) : get_block_code (words[index + 1]);

	  if (w_code == OP_BRACKET)
	    arguments_left++;
	  break;

	case OP_CURLY:
	  index += get_exp_length (words + index + 1, '}');
	  break;

	case OP_PAREN:
	  index += get_exp_length (words + index + 1, ')');
	  break;

	case OR        :
	case EQ        :
	case AND       :
	case MOD       :
	case NEQ       :
	case SET       :
	case MORE      :
	case PLUS      :
	case MINUS     :
	case DIVIDE    :
	case LESS_EQ   :
	case MORE_EQ   :
	case MULTIPLY  :
	case ADD_ASSIGN:
	case DIV_ASSIGN:
	case MOD_ASSIGN:
	case SUB_ASSIGN:
	  arguments_left += 2;
	  break;

	case FUNC_RET:
	case FUNC_OBJ:
	case IN_SCOPE:
	case SIZE:
	  arguments_left++;
	  break;

	case DEF:
	case DEFUNC:
	  for (;;)
	    {
	      token  = get_bcode_label (words[index]);
	      w_code = (token != NULL) ? get_block_code (token) : get_block_code (words[index]);
	      if (w_code == UNKNOWN || w_code == END)
		break;
	      else if (w_code == OP_CURLY)
		index += get_exp_length (words + index + 1, '}');
	      else if (w_code == OP_BRACKET || w_code == NOP_BRACKET)
		index += get_exp_length (words + index + 1, ']');
	      else if (w_code == OP_PAREN)
		index += get_exp_length (words + index + 1, ')');
	    }
	  break;

	case UNKNOWN:
	  token  = get_bcode_label (words[index + 1]);
	  w_code = (token != NULL) ? get_block_code (token) : get_block_code (words[index + 1]);

	  if (w_code == OP_BRACKET)
	    arguments_left++;
	  break;

	default:
	  break;
	}
    }

  return index;
}

FACT_t
and (func_t *scope, word_list expression)
{
  int           len          ;
  FACT_t        arg1         ;
  FACT_t        arg2         ;
  FACT_t        return_value ;
  unsigned long ip           ;

  ip = get_ip ();
  reset_ip ();
  
  expression.syntax += ip;
  expression.lines  += ip;
  
  if ((arg1 = eval (scope, expression)).type == ERROR_TYPE)
    return arg1;
  else if (arg1.type == FUNCTION_TYPE)
    return errorman_throw_reg (scope, "arguments to && must be variables");

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();

  expression.syntax += get_ip ();
  expression.lines  += get_ip ();

  len = statement_length (expression.syntax);

  if (!mpc_cmp_si (arg1.v_point->data, 0))
    {
      move_ip (ip + len);
      return return_value;
    }
  
  ip += get_ip ();
  reset_ip ();

  if ((arg2 = eval (scope, expression)).type == ERROR_TYPE)
    return arg1;
  else if (arg2.type == FUNCTION_TYPE)
    return errorman_throw_reg (scope, "arguments to && must be variables");

  if (mpc_cmp_si (arg2.v_point->data, 0) == 0)
    return return_value;

  mpc_set_si (&(return_value.v_point->data), 1);

  move_ip (ip);

  return return_value;
}

FACT_t
or (func_t *scope, word_list expression)
{
  int           len          ;
  FACT_t        arg1         ;
  FACT_t        arg2         ;
  FACT_t        return_value ;
  unsigned long ip           ;

  ip = get_ip ();
  reset_ip ();

  expression.syntax += ip;
  expression.lines  += ip;
  
  if ((arg1 = eval (scope, expression)).type == ERROR_TYPE)
    return arg1;
  else if (arg1.type == FUNCTION_TYPE)
    return errorman_throw_reg (scope, "arguments to || must be variables");

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();

  mpc_set_ui (&(return_value.v_point->data), 1);

  expression.syntax += get_ip ();
  expression.lines  += get_ip ();

  len = statement_length (expression.syntax);

  if (mpc_cmp_si (arg1.v_point->data, 0))
    {
      move_ip (ip + len);
      return return_value;
    }

  ip += get_ip ();
  reset_ip ();

  arg2 = eval (scope, expression);
  
  if ((arg2 = eval (scope, expression)).type == ERROR_TYPE)
    return arg1;
  else if (arg2.type == FUNCTION_TYPE)
    return errorman_throw_reg (scope, "arguments to || must be variables");

  if (mpc_cmp_si (arg2.v_point->data, 0) != 0)
    return return_value;

  mpc_set_ui (&(return_value.v_point->data), 1);

  move_ip (ip);

  return return_value;
}
