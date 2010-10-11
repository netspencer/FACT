#include "common.h"

a_type
new_string (func *scope, word_list expression)
{
  int length;
  int count;
  int character;

  a_type return_value;

  var *string;
  var *scroller;

  string = alloc_var ();

  return_value.type = VAR_TYPE;

  if (strcmp (expression.syntax[0], "\"") == 0)
    return_value.v_point = string;
  else
    {
      scroller = string;

      expression.move_forward[0] = true;

      for (length = 1, count = 0; expression.syntax[0][count] != '\0'; length++, count++, scroller = scroller->next)
	{
	stringer:
	  character = expression.syntax[0][count];

	  if (character == '\\')
            {
              switch (expression.syntax[0][++count])
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
      
      if (expression.syntax[2] != NULL && !strcmp (expression.syntax[2], "\""))
	{
	  expression.move_forward[1] = true;
	  expression.move_forward[2] = true;
	  expression.move_forward += 3;
	  expression.syntax += 3;

	  count = 0;
	  goto stringer;
	}
      
      return_value.v_point = alloc_var ();
      return_value.v_point->array_size = length;
      return_value.v_point->array_up = string;
    }

  expression.move_forward[0] = expression.move_forward[1] = true;

  return return_value;
}

a_type
print_character (func *scope, word_list expression)
{
  a_type evald;

  evald = eval (scope, expression);

  if (evald.type == FUNCTION_TYPE)
    return errorman_throw_reg (scope, "cannot print a function");
  
  if (evald.type == VAR_TYPE)
    putchar (mpc_get_si (evald.v_point->data));
  
  return evald;
}

a_type
input_character (func *scope, word_list expression)
{
  a_type return_value;

  return_value.type = VAR_TYPE;

  return_value.v_point = alloc_var ();

  mpc_set_si (&(return_value.v_point->data), fgetc (stdin));

  return return_value;
}

a_type print_var (func *scope, word_list expression)
{
  a_type evald;

  evald = eval (scope, expression);

  if (evald.type == VAR_TYPE)
    printf ("%s", mpc_get_str (evald.v_point->data));

  return evald;
}
