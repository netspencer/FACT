#include "execfile.h"

a_type
run_file (func *scope, const char *filename)
{
  unsigned int line_num;
  
  char *input;
  char **parsed_input;

  FILE *fp;

  linked_word *formatted;

  a_type returned;

  printf ("Opening file <%s>\n", filename);

  fp = fopen (filename, "r");

  if (fp == NULL)
    return errorman_throw_reg (scope, "could not open file");

  line_num = 1;

  for (;;)
    {
      input = get_input (fp, &line_num);

      if (input == NULL)
	{
	  fclose (fp);
	  printf ("Closing file <%s>.\n", filename);
	  returned.type = VAR_TYPE;
	  returned.isret = false;
	  return returned;
	}

      parsed_input = get_words (input);

      if (parsed_input == NULL)
	{
	  fclose (fp);
	  printf ("Closing file <%s>.\n", filename);
	  returned.type = VAR_TYPE;
	  return returned;
	}

      formatted = create_list (parsed_input);

      formatted = set_list (formatted, END);

      while (formatted->previous != NULL)
	formatted = formatted->previous;

      rev_shunting_yard (formatted);

      while (formatted->previous != NULL)
	formatted = formatted->previous;

      set_link (formatted);
      parsed_input = convert_link (formatted);

      returned = expression (scope, parsed_input);

      if (returned.type == ERROR_TYPE)
        {
          errorman_dump (returned.error, line_num, filename);
          continue;
        }

      if (returned.isret == true)
        {
	  fclose (fp);
	  printf ("Closing file <%s>.\n", filename);
	  return returned;
        }
    }
}
