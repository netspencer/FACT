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
  unsigned int    line_num;

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

  line_num = 1;

  for (;;)
    {
      input = get_input (fp, &line_num);
      
#ifdef DEBUG
      printf (":RAW:\n%s\n:END RAW:\n", input);
#endif

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

      parsed_input = get_words (input);

      
#ifdef DEBUG
      puts ("WORDS: ");
      for (print_parsed = 0; parsed_input[print_parsed]; print_parsed++)
	printf ("'%s' ", parsed_input[print_parsed]);
      putchar ('\n');
#endif


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
#ifdef PARSE_CHECK
      if (parsing_error (formatted, false, 0))
	printf ("Parsing error: %s.\n", parsing_get_error ());
#endif
      for (rev_shunting_yard (formatted); formatted->previous != NULL; formatted = formatted->previous);
      set_link (formatted);
      parsed_input = convert_link (formatted);

#ifdef DEBUG
      puts ("PARSED: ");
      for (print_parsed = 0; parsed_input[print_parsed]; print_parsed++)
	printf ("'%s' ", parsed_input[print_parsed]);
      putchar ('\n');
#endif
      
      returned = eval_expression (scope, make_word_list (parsed_input, true));

      if (returned.type == ERROR_TYPE)
        {
          errorman_dump (returned.error, line_num, filename);
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
