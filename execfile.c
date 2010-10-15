#include "execfile.h"

FACT_t
run_file (func_t *scope, const char *filename, bool silent)
{
  unsigned int line_num;
  //  int read;
  
  char *input;
  char **parsed_input;

  FILE *fp;

  linked_word *formatted;

  FACT_t returned;

  if (!silent)
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

	  if (!silent)
	    printf ("Closing file <%s>.\n", filename);

	  returned.type = VAR_TYPE;
	  returned.isret = false;
	  return returned;
	}

      parsed_input = get_words (input);

      if (parsed_input == NULL)
	{
	  fclose (fp);

	  if (!silent)
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

      /*
      for (read = 0; parsed_input[read] != NULL; read++)
	printf ("%s ", parsed_input[read]);
      putchar ('\n');
      */

      returned = expression (scope, parsed_input);

      if (returned.type == ERROR_TYPE)
        {
          errorman_dump (returned.error, line_num, filename);
          continue;
        }

      if (returned.isret == true)
        {
	  fclose (fp);

	  if (!silent)
	    printf ("Closing file <%s>.\n", filename);
	  return returned;
        }
    }
}
