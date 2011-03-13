#include "FACT.h"

FACT_t
run_file (func_t *scope, const char *filename, bool silent)
{
  int             print_parsed;
  int             hold_line;
#ifdef DEBUG
  char         *  scroll_through;
#endif
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

  fp               = fopen (filename, "r");
  hold_line        = scope->line;
  scope->line      = 1;
  hold_fn          = scope->name;
  scope->name      = (char *) filename;
  scope->file_name = (char *) filename;

  if (fp == NULL)
    {
      scope->name = hold_fn;
      return errorman_throw_reg (scope, "could not open file");
    }

  end_line = 1;

  for (;;)
    {
      scope->line = end_line;
      input       = get_input (fp, &end_line, NULL, NULL);

#ifdef DEBUG_INPUT
      puts ("\ninput:");
      scroll_through = input;
      do
	{
	  printf (":\t");
	  while (scroll_through != NULL && *scroll_through != '\n' && *scroll_through != '\0')
	    putchar (*scroll_through++);
	  if (scroll_through != NULL && *scroll_through == '\n')
	    scroll_through++;
	  putchar ('\n');
	}
      while (scroll_through != NULL &&  *scroll_through != '\0');
      fflush (stdout);
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
      formatted = set_list (formatted, END);
      for (formatted = set_list (formatted, END); formatted->previous != NULL; formatted = formatted->previous);

      check_line = end_line;
      if (check_for_errors (formatted, 0, &check_line, false))
	{
	  print_parsing_error ("stdin", hold_line);
	  continue;
	}

      for (rev_shunting_yard (formatted); formatted->previous != NULL; formatted = formatted->previous);
      
      set_link (formatted);
      parsed_input = convert_link (formatted);
      compile_to_bytecode (parsed_input);
      reset_ip ();

      returned = eval_expression (scope, make_word_list (parsed_input, true));

      if (returned.type == ERROR_TYPE)
        {
          errorman_dump (returned.error);
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

#ifdef LOAD_FILE_FIXED
FACT_t 
run_file (func_t *scope, const char *file_name, bool silent)
{
  /**
   * run_file - load and run a file.
   *
   * @silent: whether or not to print trivial info.
   * 
   * @TODO: Currently we just load the entire file into memory and parse it. This
   * should probably be fixed so that only parts of the file are loaded at a time.
   * Right now though, this works fine.
   */
  int i, c;
  int hold_line;  

  char        *contents;   // Contents of the file.
  char        *parsed;     // Contents of the file, parsed.
  char        *hold_fname; // Holds the old filename.
  FILE        *fp;
  linked_word *formatted;

  if (!silent)
    printf ("Opening file <%s>.\n", file_name);

  if ((fp = fopen (filename, "r")) == NULL)
    errorman_throw_catchable (scope, "could not open file");

  hold_line   = scope->line;
  scope->line = 1;
  hold_fname  = scope->name;
  scope->name = scope->file_name = (char *) file_name;

  // Load all of the contents of the file into contents.
  contents = NULL;
  for (i = 0; (c = fgetc (fp)) != EOF; i++)
    {
      contents = better_realloc (contents, sizeof (char) * (i + 1));
      contents[i] = c;
    }
  
}
#endif
