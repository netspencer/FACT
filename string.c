#include "interpreter.h"

a_type new_string (func *scope, char **words)
{
  a_type return_value;
  var *string;
  var *scroller;
  int length;
  int count;
  int character;
  int pos;
  int increase;

  increase = 1;

  string = alloc_var ();

  return_value.type = VAR_TYPE;

  if (strcmp (words[0], "\"") == 0)
    {
      string->array_size = 1;
      return_value.v_point = string;
    }
  else
    {
      scroller = string;

      for (length = 1, count = 0; words[0][count] != '\0'; length++, count++, scroller = scroller->next)
	{
	stringer:
	  character = words[0][count];

	  if (character == '\\')
            {
              switch (words[0][++count])
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
		case '"':
		  character = '"';
		  break;
		default:
		  character = '\\';
		  break;
                }
	      
            }
	  mpz_set_si (scroller->data, character);
	  scroller->next = alloc_var ();
	}
      
      if (words[2] != NULL && !strcmp (words[2], "\""))
	{
	  increase++;
	  words += 3;
	  count = 0;
	  goto stringer;
	}
      
      return_value.v_point = alloc_var ();
      return_value.v_point->array_size = length;
      return_value.v_point->array_up = string;
    }

  /*
  printf ("words[0] = %s, words[increase] = %s, increase = %d\n", words[0], words[increase], increase);

  for (pos = 0; words[pos] != NULL; pos++)
    words[pos] = words[pos + increase];
  */

  for (pos = 0; words[pos] != NULL; pos++)
    words[pos] = words[pos + 2];

  return return_value;
}

a_type print_character (func *scope, char **words)
{
  a_type evald;
  int pos;

  evald = eval (scope, words);

  if (evald.type == FUNCTION_TYPE)
    return errorman_throw_reg (scope, "cannot print a function");
  
  if (evald.type == VAR_TYPE)
    putchar (mpz_get_si (evald.v_point->data));
  
  for (pos = 0; words[pos] != NULL; pos++)
    words[pos] = words[pos + 1];    

  return evald;
}
a_type
input_character (func *scope, char **words)
{
  a_type return_value;
  int pos;

  return_value.type = VAR_TYPE;

  return_value.v_point = alloc_var ();

  mpz_set_si (return_value.v_point->data, getchar ());

  for (pos = 0; words[pos] != NULL; pos++)
    words[pos] = words[pos + 1];

  return return_value;
}

a_type print_var (func *scope, char **words)
{
  a_type evald;
  int pos;

  evald = eval (scope, words);

  switch (evald.type)
    {
    case ERROR_TYPE:
      break;
    case VAR_TYPE:
      gmp_printf ("%Zd", evald.v_point->data);
    case FUNCTION_TYPE:
      for (pos = 0; words[pos] != NULL; pos++)
	words[pos] = words[pos + 1];
      break;
    default:
      break;
    }

  return evald;
}
