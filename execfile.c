#include "execfile.h"

FACT_t
run_file (func_t *scope, const char *filename, bool silent)
{
  int             print_parsed;
  int             hold_line;
  char         *  hold_fn;
  char         *  input;
  char         ** parsed_input;
  FILE         *  fp;
  FACT_t          returned;
  linked_word  *  formatted;
  unsigned int    end_line;
  unsigned int    check_line;

  if (!silent)
    printf ("Opening file <%s>\n", filename);

  fp          = fopen (filename, "r");
  hold_line   = scope->line;
  scope->line = 1;
  hold_fn     = scope->name;
  scope->name = (char *) filename;

  if (fp == NULL)
    {
      return errorman_throw_reg (scope, "could not open file");
      scope->name = hold_fn;
    }

  end_line = 1;

  for (;;)
    {
      scope->line = end_line;
      input       = get_input (fp, &end_line);

      if (input == NULL)
	{
	  fclose (fp);

	  if (!silent)
	    printf ("Closing file <%s>.\n", filename);

	  returned.type          = VAR_TYPE;
	  returned.return_signal = false;
	  scope->name            = hold_fn;
	  scope->line            = hold_line;
	  return returned;
	}

      if (check_for_incompletions (filename, input))
	continue;

      parsed_input = get_words (input);

      if (parsed_input == NULL)
	{
	  fclose (fp);

	  if (!silent)
	    printf ("Closing file <%s>.\n", filename);

	  returned.type = VAR_TYPE;
	  scope->name   = hold_fn;
	  scope->line   = hold_line;
	  return returned;
	}

      formatted = create_list (parsed_input);
      for (formatted = set_list (formatted, END); formatted->previous != NULL; formatted = formatted->previous);

      /* ---- Check for parsing errors. ---- */
      check_line = end_line;
      if (check_for_errors (formatted, 0, &check_line))
	{
	  print_parsing_error (filename, check_line);
	  continue;
	}

      for (rev_shunting_yard (formatted); formatted->previous != NULL; formatted = formatted->previous);
      set_link (formatted);
      parsed_input = convert_link (formatted);

      returned = eval_expression (scope, make_word_list (parsed_input, true));

      if (returned.type == ERROR_TYPE)
        {
          errorman_dump (returned.error, returned.error.scope->line, filename);
          continue;
        }

      if (returned.return_signal == true)
        {
	  fclose (fp);

	  if (!silent)
	    printf ("Closing file <%s>.\n", filename);
	  scope->name = hold_fn;
	  scope->line = hold_line;
	  return returned;
        }
    }
}
