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
	in_quotes = !in_quotes;

      if (c == ';' || c == '}')
	{
	  input[count] = '\0';

	  if (paren_count == 0 && bracket_count == 0 && curly_count == 0 && !in_quotes
	      /*&& (input[count - 1] == ';' || input[count - 1] == '}')*/)
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
  unsigned int    line_num;

  /* Print out the disclaimer and logo. */
  printf ("The FACT programming language interactive shell\n(c) 2010 Matthew Plant, under the GPL version 3.\n");
  print_logo ();
  line_num = 1;
  end_line = 1;

  for (;;)
    {
      main_scope->line = end_line;
      input            = get_input (stdin, &end_line);
      
#ifdef DEBUG
      printf (":RAW:\n%s\n:END RAW:\n", input);
#endif
      
      /* Check 1... */
      if (input == NULL)
        {
          printf ("\nExiting...\n");
	  return;
        }
      
      parsed_input = get_words (input);

#ifdef DEBUG
      puts ("WORDS: ");
      for (print_parsed = 0; parsed_input[print_parsed]; print_parsed++)
	printf ("'%s' ", parsed_input[print_parsed]);
      putchar ('\n');
#endif

      /* Check 2... */
      if (parsed_input == NULL)
	{
	  printf ("\nExiting...\n");
	  return;
	}
      
      formatted = create_list (parsed_input);
      for (formatted = set_list (formatted, END); formatted->previous != NULL; formatted = formatted->previous);
#ifdef PARSE_CHECK
      line_num = main_scope->line;
      if (check_for_errors (formatted, 0, &line_num))
	printf ("PARSING ERROR [%d]: %s.\n", line_num, get_error ());
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
      returned = eval_expression (main_scope, make_word_list (parsed_input, true));
      if (returned.type == ERROR_TYPE)
        {
	  errorman_dump (returned.error, returned.error.scope->line, "stdin");
          continue;
        }
      if (returned.type == VAR_TYPE)
	printf ("Returned value: %s\n", mpc_get_str (returned.v_point->data));
      else
#ifdef DEBUG
	{
#endif
	  printf ("Returned object [%s]\n", returned.f_point->name);
#ifdef DEBUG
	  printf ("Object starts at line [%d]\n", returned.f_point->line);
	}
#endif
      if (returned.return_signal == true)
        {
          printf ("Exiting...\n");
	  return;
        }
#ifdef DEBUG
      printf ("Now on line [scope:%d], [flat:%d]\n", main_scope->line, line_num);
#endif
    }
}
