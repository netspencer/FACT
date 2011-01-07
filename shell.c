#include "shell.h"

static void
print_logo ()
{
  printf (" ________   ________   ________   _________\n"
	  "/\\  _____\\ /\\  ___  \\ /\\  _____\\ /\\___  ___\\\n"
	  "\\ \\ \\____/ \\ \\ \\__L\\ \\\\ \\ \\____/ \\/__/\\ \\__/\n"
	  " \\ \\  ___\\  \\ \\  ____ \\\\ \\ \\         \\ \\ \\\n"
	  "  \\ \\ \\__/   \\ \\ \\__/\\ \\\\ \\ \\______   \\ \\ \\\n"
	  "   \\ \\_\\      \\ \\_\\ \\ \\_\\\\ \\_______\\   \\ \\_\\\n"
	  "    \\/_/       \\/_/  \\/_/ \\/_______/    \\/_/\n");
}

char *
get_input (FILE *fp, unsigned int *line_number)
{
  int    count;
  int    paren_count;
  int    bracket_count;
  int    curly_count;
  int    c;
  bool   in_quotes;  
  char * input;

  if (fp == stdin)
    printf ("%% ");

  for (count = 1, input = NULL, in_quotes = false,
	 paren_count = bracket_count = curly_count = 0; (c = fgetc (fp)) != EOF; count++)
    {
      input = (char *) better_realloc (input, (count + 1) * sizeof (char));

      if (c == '#' && !in_quotes)
	{
	  while ((c = fgetc (fp)) != EOF && c != '\n')
	    ;
	  ungetc (c, fp);
	  count--;
	  continue;
	}

      if (c == '\n')
	{
	  ++*line_number;
	  if (count > 1 && fp == stdin
	      && ((in_quotes || paren_count || bracket_count || curly_count)
		  || (input[count - 2] != ';' || input[count - 2] != '}')))
	    printf (": ");
	}
	      
      input[count - 1] = c;

      if (!in_quotes)
	{
	  if (c == '(')
	    paren_count++;
	  else if (c == ')')
	    paren_count--;
	  else if (c == '[')
	    bracket_count++;
	  else if (c == ']')
	    bracket_count--;
	  else if (c == '{')
	    curly_count++;
	  else if (c == '}')
	    curly_count--;
	}

      if (c == '"')
	{
	  if (in_quotes)
	    {
	      if (input[count - 2] != '\\'
		  || (count > 3 && input[count - 3] == '\\'))
		in_quotes = false;
	    }
	  else
	    in_quotes = true;
	}
      if (c == ';' || c == '}')
	{
	  input[count] = '\0';

	  if (paren_count == 0 && bracket_count == 0 && curly_count == 0 && !in_quotes)
	    break; 
	}      
    }
  
  if (input != NULL)
    {
      input            = better_realloc (input, sizeof (char) * (count + 2));
      input[count]     = c;
      input[count + 1] = '\0';
    }

  for (count--; input != NULL && count >= 0; count--)
    {
      if (!isspace ((int) input[count]) && input[count] != '\0')
	return input;
    }
  
  return NULL;
}

void
shell (func_t *main_scope)
{
  int             print_parsed;
  char         *  input;
  char         ** parsed_input;
  FACT_t          returned;
  linked_word  *  formatted;
  unsigned int    end_line;
  unsigned int    hold_line;

  /* Print out the disclaimer and logo. */
  printf ("The FACT programming language interactive shell\n(c) 2010 Matthew Plant, under the GPL version 3.\n");
  print_logo ();
  
  end_line              = 1;
  main_scope->file_name = "stdin";

  for (;;)
    {
      main_scope->line = end_line;
      input            = get_input (stdin, &end_line);

      /* ---- Exit check #1 ---- */
      if (input == NULL)
        {
          printf ("\nExiting...\n");
	  return;
        }
      /* ---- Check for incompletions ---- */
      if (check_for_incompletions ("stdin", input))
	continue;
      /* ---- Tokenize the input ---- */
      parsed_input = get_words (input);
      /* ---- Exit check #2 ---- */
      if (parsed_input == NULL)
	{
	  printf ("\nExiting...\n");
	  return;
	}
      /* ---- Continue to parse. ---- */
      formatted = create_list (parsed_input);
      for (formatted = set_list (formatted, END); formatted->previous != NULL; formatted = formatted->previous);
      /* ---- Check for parsing errors. ---- */
      hold_line = end_line;
      if (check_for_errors (formatted, 0, &hold_line))
	{
	  print_parsing_error ("stdin", hold_line);
	  continue;
	}
      /* ---- Continue to parse; convert to polish notation ---- */
      for (rev_shunting_yard (formatted); formatted->previous != NULL; formatted = formatted->previous);
      set_link (formatted);
      /* ---- Convert the list to a char ** ---- */
      parsed_input = convert_link (formatted);
      /* ---- Evaluate the statement ---- */
      returned = eval_expression (main_scope, make_word_list (parsed_input, true));
      if (returned.type == ERROR_TYPE)
        {
	  errorman_dump (returned.error, returned.error.scope->line, "stdin", returned.error.scope->file_name);
          continue;
        }
      if (returned.type == VAR_TYPE)
	printf ("Returned value: %s\n", mpc_get_str (returned.v_point->data));
      else
	printf ("Returned object [%s]\n", returned.f_point->name);
      if (returned.return_signal == true)
        {
          printf ("Exiting...\n");
	  return;
        }
    }
}
