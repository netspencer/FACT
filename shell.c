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
  int    index;
  int    p_count;
  int    b_count;
  int    c_count;
  int    curr_char;
  bool   in_quotes;  
  char * input;

  if (fp == stdin)
    printf ("%% ");

  /* Doing var = var = x; is unclear and annoying. */
  input     = NULL;
  p_count   = 0;
  b_count   = 0;
  c_count   = 0;
  in_quotes = false;
  
  for (index = 1; (curr_char = fgetc (fp)) != EOF; index++)
    {
      if (curr_char == '#' && !in_quotes)
	{
	  while ((curr_char = fgetc (fp)) != EOF && curr_char != '\n')
	    ;
	  ungetc (curr_char, fp);
	  index--;
	  continue;
	}
      if (curr_char == '\n')
	{
	  ++*line_number;
	  if (index > 1 && fp == stdin
	      && ((in_quotes || p_count || b_count || c_count)
		  || (input[index - 2] != ';' || input[index - 2] != '}')))
	    printf (": ");
	}

      input            = better_realloc (input, (index + 1) * sizeof (char));
      input[index - 1] = curr_char;

      if (!in_quotes)
	{
	  if (curr_char == '(')
	    p_count++;
	  else if (curr_char == ')')
	    p_count--;
	  else if (curr_char == '[')
	    b_count++;
	  else if (curr_char == ']')
	    b_count--;
	  else if (curr_char == '{')
	    c_count++;
	  else if (curr_char == '}')
	    c_count--;
	}
      if (curr_char == '"')
	{
	  if (in_quotes)
	    {
	      if (input[index - 2] != '\\'
		  || (index > 3 && input[index - 3] == '\\'))
		in_quotes = false;
	    }
	  else
	    in_quotes = true;
	}
      if (curr_char == ';' || curr_char == '}')
	{
	  input[index] = '\0';

	  if (p_count == 0 && b_count == 0 && c_count == 0 && !in_quotes)
	    break; 
	}      
    }
  
  if (input != NULL)
    {
      input            = better_realloc (input, sizeof (char) * (index + 2));
      input[index]     = curr_char;
      input[index + 1] = '\0';
      for (index--; index >= 0; index--)
	{
	  if (!isspace ((int) input[index]) && input[index] != '\0')
	    return input;
	}  
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
      compile_to_bytecode (parsed_input);
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
