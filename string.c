#include "string.h"

FACT_t
new_string (func_t *scope, word_list expression)
{
  int            character;
  mpz_t          length;
  var_t        * string;
  var_t        * scroller;
  FACT_t         return_value;
  unsigned int   index;

  string            = alloc_var ();
  return_value.type = VAR_TYPE;

  expression.syntax += get_ip ();
  expression.lines  += get_ip ();
  
  if (tokcmp (expression.syntax[0], "\"") == 0)
    return_value.v_point = string;
  else
    {
      scroller = string;

      for (mpz_init (length), index = 0; expression.syntax[0][index] != '\0'; mpz_add_ui (length, length, 1), index++, scroller = scroller->next)
	{
	  character = expression.syntax[0][index];

	  if (character == '\\')
            {
              switch (expression.syntax[0][++index])
                {
		case 'n':
		  character = '\n';
		  break;
		  
		case 't':
		  character = '\t';
		  break;
		  
		case 'v':
		  character = '\v';
		  break;
		  
		case 'r':
		  character = '\r';
		  break;

		case '0':
		  character = '\0';
		  break;
		  
		case '"':
		  character = '"';
		  break;
		  
		default:
		  character = '\\';
		  break;
                }
            }
	  mpc_set_si (&(scroller->data), character);
	  scroller->next = alloc_var ();
	}
      free_var (scroller->next);
      scroller->next = NULL;
      if (!mpz_cmp_ui (length, 1))
	return_value.v_point = string;
      else
	{
	  return_value.v_point           = alloc_var ();
	  return_value.v_point->array_up = string;
	}
      mpz_set (return_value.v_point->array_size, length);
    }
  move_ip (2);

  return return_value;
}

FACT_t
print_character (func_t *scope, word_list expression)
{
  FACT_t evald;

  evald = eval (scope, expression);

  if (evald.type == FUNCTION_TYPE)
    return errorman_throw_reg (scope, "cannot print a function");
  
  if (evald.type == VAR_TYPE)
    putchar (mpc_get_si (evald.v_point->data));
  
  return evald;
}

FACT_t
input_character (func_t *scope, word_list expression)
{
  FACT_t return_value;

  return_value.type    = VAR_TYPE;
  return_value.v_point = alloc_var ();

  mpc_set_si (&(return_value.v_point->data), fgetc (stdin));

  return return_value;
}

FACT_t print_var (func_t *scope, word_list expression)
{
  FACT_t evald;

  evald = eval (scope, expression);

  if (evald.type == VAR_TYPE)
    printf ("%s", mpc_get_str (evald.v_point->data));

  return evald;
}
