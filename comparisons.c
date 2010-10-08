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

a_type
equal (a_type arg1, a_type arg2)
{
  a_type return_value;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();
    
  if (arg1.type != arg2.type)
    return errorman_throw_reg (NULL, "argument types to == do not match");
  
  if (arg1.type == VAR_TYPE)
    {
      if (mpc_cmp (arg1.v_point->data, arg2.v_point->data) == 0)
        mpc_set_si (&(return_value.v_point->data), 1);
    }
  else /* this block does not work, will be fixed when I give a damn */ 
    {
      if (arg1.f_point == arg2.f_point)
        mpc_set_si (&(return_value.v_point->data), 1);
    }

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * *
 * not_equal: checks if two arguments are    *
 * not equal to each other.                  *
 * * * * * * * * * * * * * * * * * * * * * * */

a_type
not_equal (a_type arg1, a_type arg2)
{
  a_type return_value;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (arg1.type != arg2.type)
    return errorman_throw_reg (NULL, "argument types to != do not match");
    
  if (arg1.type == VAR_TYPE)
    {
      if (mpc_cmp (arg1.v_point->data, arg2.v_point->data) != 0)
        mpc_set_si (&(return_value.v_point->data), 1);
    }
  else /* this block does not work, will be fixed when I give a damn */ 
    {
      if (arg1.f_point != arg2.f_point)
        mpc_set_si (&(return_value.v_point->data), 1);
    }

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * *
 * more: checks if the first argument is     *
 * greater in value than the other arguemnt. *
 * If either argument is not a variable, it  *
 * throws an error.                          *
 * * * * * * * * * * * * * * * * * * * * * * */

a_type
more (a_type arg1, a_type arg2)
{
  a_type return_value;

  return_value.type = VAR_TYPE;
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
 * variable, it throws an error.             *
 * * * * * * * * * * * * * * * * * * * * * * */

a_type
more_equal (a_type arg1, a_type arg2)
{
  a_type return_value;

  return_value.type = VAR_TYPE;
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

a_type
less (a_type arg1, a_type arg2)
{
  a_type return_value;
  
  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (NULL, "arguments to < must be vars");

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

a_type
less_equal (a_type arg1, a_type arg2)
{
  a_type return_value;

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (arg1.type != VAR_TYPE || arg2.type != VAR_TYPE)
    return errorman_throw_reg (NULL, "arguments to <= must be vars");

  if (mpc_cmp (arg1.v_point->data, arg2.v_point->data) <= 0)
    mpc_set_si (&(return_value.v_point->data), 1);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * * *
 * This function is for checking how long an *
 * expression is. It's used by the "and"     *
 * function. It's not very good but works I  *
 * guess.                                    *
 * * * * * * * * * * * * * * * * * * * * * * */

static int
statement_length (char **words)
{
  int pos;
  int w_code;
  int arguments_left;

  extern int get_exp_length_first (char **, int);
  extern word_code get_block_code (char *);

  for (pos = 0, arguments_left = 1; arguments_left > 0 && words[pos] != NULL; pos++, arguments_left--)
    {
      switch (w_code = get_block_code (words[pos]))
	{
	case CL_PAREN:
	case CL_BRACKET:
	case CL_CURLY:
	  return pos; 

	case OP_BRACKET:
	case NOP_BRACKET:
	  pos += get_exp_length (words + pos + 1, ']');
	  //	  printf (":: %s\n", words[pos]);

	  if (get_block_code (words[pos + 1]) == OP_BRACKET)
	    arguments_left++;
	  break;

	case OP_CURLY: /* we are assuming that the user is an idiot and we have to check for this */
	  pos += get_exp_length (words + pos + 1, '}');
	  //	  arguments_left++;
	  break;

	case OP_PAREN:
	  pos += get_exp_length (words + pos + 1, ')');
	  printf (":: %s\n", words[pos]);
	  //	  arguments_left++;
	  break;

	case SET:
	case PLUS:
	case MINUS:
	case MULTIPLY:
	case DIVIDE:
	case MOD:
	case ADD_ASSIGN:
	case SUB_ASSIGN:
	case DIV_ASSIGN:
	case MOD_ASSIGN:
	case AND:
	case OR:
	case EQ:
	case NEQ:
	case MORE:
	case LESS_EQ:
	case MORE_EQ:
	  arguments_left += 2;
	  break;
	  
	case FUNC_RET:
	case FUNC_OBJ:
	  while ((w_code = get_block_code (words[++pos])) != FUNC_END
		 && w_code != END)
	    {
	      if (w_code == OP_CURLY)
		pos += get_exp_length (words + pos + 1, '}');
	      else if (w_code == OP_BRACKET || w_code == NOP_BRACKET)
		pos += get_exp_length (words + pos + 1, ']');
	      else if (w_code == OP_PAREN)
		pos += get_exp_length (words + pos + 1, ')');
	    }
	  break;

	case IN_SCOPE:
	case SIZE:
	  arguments_left++;
	  break;

	case DEF:
	case DEFUNC:
	  while ((w_code = get_block_code (words[++pos])) != UNKNOWN
		 && w_code != END)
	    {
	      if (w_code == OP_CURLY)
		pos += get_exp_length (words + pos + 1, '}');
	      else if (w_code == OP_BRACKET || w_code == NOP_BRACKET)
		pos += get_exp_length (words + pos + 1, ']');
	      else if (w_code == OP_PAREN)
		pos += get_exp_length (words + pos + 1, ')');
	    }
	  break;

	case UNKNOWN:
	  if (!strcmp (words[pos], "?")
	      || get_block_code (words[pos + 1]) == OP_BRACKET)
	    arguments_left++;
	  break;

	default:
	  break;
	}
    }

  return pos;
}

/* * * * * * * * * * * * * * * * * * * * * * *
 * and: checks if both arguments are true.   *
 * In order for an argument to be true it    *
 * must not be equal to zero. This function  *
 * needs to be treaty differently than other *
 * math functions, so that it doesn't return *
 * an error if the second argument is        *
 * erroneous and the first argument          *
 * evaluates to false.                       *
 * * * * * * * * * * * * * * * * * * * * * * */

a_type
and (func *scope, word_list expression)
{
  int pos;
  int len;

  a_type arg1;
  a_type arg2;
  a_type return_value;

  arg1 = eval (scope, expression);

  if (arg1.type == ERROR_TYPE)
    return arg1;
  else if (arg1.type == FUNCTION_TYPE)
    return errorman_throw_reg (scope, "arguments to && must be variables");

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  while (expression.move_forward[0])
    {
      expression.syntax++;
      expression.move_forward++;
    }

  len = statement_length (expression.syntax) + 1;

  if (mpc_cmp_si (arg1.v_point->data, 0) == 0)
    {
      for (pos = 0; pos < len; pos++)
	expression.move_forward[pos] = true;

      return return_value;
    }

  arg2 = eval (scope, expression);
  
  if (arg2.type == ERROR_TYPE)
    return arg1;
  else if (arg2.type == FUNCTION_TYPE)
    return errorman_throw_reg (scope, "arguments to && must be variables");

  if (mpc_cmp_si (arg2.v_point->data, 0) == 0)
    return return_value;

  mpc_set_si (&(return_value.v_point->data), 1);

  return return_value;
}

/* * * * * * * * * * * * * * * * * * * * * *
 * or: checks if at least one of the       *
 * passed arguments evaluate to true.      *
 * * * * * * * * * * * * * * * * * * * * * */

a_type
or
(func *scope, word_list expression)
{
  int pos;
  int len;

  a_type arg1;
  a_type arg2;
  a_type return_value;

  arg1 = eval (scope, expression);

  if (arg1.type == ERROR_TYPE)
    return arg1;
  else if (arg1.type == FUNCTION_TYPE)
    return errorman_throw_reg (scope, "arguments to || must be variables");

  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  mpc_set_ui (&(return_value.v_point->data), 1);

  while (expression.move_forward[0])
    {
      expression.syntax++;
      expression.move_forward++;
    }

  len = statement_length (expression.syntax) + 1;

  if (mpc_cmp_si (arg1.v_point->data, 0) != 0)
    {
      for (pos = 0; pos < len; pos++)
	expression.move_forward[pos] = true;

      return return_value;
    }

  arg2 = eval (scope, expression);
  
  if (arg2.type == ERROR_TYPE)
    return arg1;
  else if (arg2.type == FUNCTION_TYPE)
    return errorman_throw_reg (scope, "arguments to || must be variables");

  if (mpc_cmp_si (arg2.v_point->data, 0) != 0)
    return return_value;

  mpc_set_ui (&(return_value.v_point->data), 1);

  return return_value;
}
