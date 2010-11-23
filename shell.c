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

  for (count = 1, input = NULL, in_quotes = false,
	 paren_count = bracket_count = curly_count = 0; (c = fgetc (fp)) != EOF; count++)
    {
      input = (char *) better_realloc (input, (count + 1) * sizeof (char));

      if (c == '#' && !in_quotes)
	{
	  while ((c = fgetc (fp)) != EOF && c != '\n')
	    ;
	  count--;
	  continue;
	}

      if (c != '\n' || in_quotes)
 	input[count - 1] = c;
      else
	{
	  ungetc (' ', fp);
	  count--;
	  (*line_number)++;
	}

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
	in_quotes = !in_quotes;

      if (c == ';' || c == '}')
	{
	  input[count] = '\0';

	  if (paren_count == 0 && bracket_count == 0 && curly_count == 0 && !in_quotes
	      && (input[count - 1] == ';' || input[count - 1] == '}'))
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
  unsigned int    line_num;

  /* Print out the disclaimer and logo. */
  printf ("The FACT programming language interactive shell\n(c) 2010 Matthew Plant, under the GPL version 3.\n");
  print_logo ();
  line_num = 1;

  for (;;)
    {
      input = get_input (stdin, &line_num);

      /* Check 1... */
      if (input == NULL)
        {
          printf ("\nExiting...\n");
	  return;
        }
      
      parsed_input = get_words (input);

      /* Check 2... */
      if (parsed_input == NULL)
	{
	  printf ("\nExiting...\n");
	  return;
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
      returned = expression (main_scope, parsed_input);
      if (returned.type == ERROR_TYPE)
        {
          errorman_dump (returned.error, line_num, "stdin");
          continue;
        }
      if (returned.type == VAR_TYPE)
	printf ("Returned value: %s\n", mpc_get_str (returned.v_point->data));
      else
        printf ("Returned object [%s]\n", returned.f_point->name);
      if (returned.isret == true)
        {
          printf ("Exiting...\n");
	  return;
        }
    }
}
