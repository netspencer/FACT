#include "shell.h"

static inline void
print_logo ( void )
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

/*
void
shell (func_t *main_scope)
{
  int             print_parsed;
  char         *  input;
  char         ** scroll_through;
  char         ** parsed_input;
  FACT_t          returned;
  linked_word  *  formatted;
  unsigned int    end_line;
  unsigned int    hold_line;

  printf ("The FACT programming language interactive shell\n(c) 2010 Matthew Plant, under the GPL version 3.\n");
  print_logo ();
  
  end_line              = 1;
  main_scope->file_name = "stdin";

  for (;;)
    {
      main_scope->line = end_line;
      input            = get_input (stdin, &end_line);
      
      if (input == NULL)
        {
          printf ("\nExiting...\n");
	  return;
        }

      if (check_for_incompletions ("stdin", input))
	continue;
      
      parsed_input = get_words (input);
      
      if (parsed_input == NULL)
	{
	  printf ("\nExiting...\n");
	  return;
	}
      
      formatted = create_list (parsed_input);
      for (formatted = set_list (formatted, END); formatted->previous != NULL; formatted = formatted->previous);
      
      hold_line = end_line;
      if (check_for_errors (formatted, 0, &hold_line, false))
	{
	  print_parsing_error ("stdin", hold_line);
	  continue;
	}

      for (rev_shunting_yard (formatted); formatted->previous != NULL; formatted = formatted->previous);
      
      set_link (formatted);      
      parsed_input = convert_link (formatted);

      puts ("\ninput:");
      scroll_through = parsed_input;
      while (scroll_through[0] != NULL)
	{
	  puts (scroll_through[0]);
	  scroll_through++;
	}
      fflush (stdout);

      compile_to_bytecode (parsed_input);
      reset_ip ();
      
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
*/

void
shell (func_t * scope)
{
  /* shell - when FACT is run by default it comes to here. The
   * main user interface for FACT, this function grabs an
   * expression of input, parses it, runs it, and repeats. It
   * will continue to do so until the user does a C-d, C-c,
   * returns a value from the main scope, or calls the exit
   * function.
   *
   * @scope  - the scope to use when evaluating expressions.
   */
  char   *  input     ;
  char   ** tokenized ;
  byte   ** bytecode  ; /* Complete string to be passed to the
			 * interpreter.                           */
  FACT_t    returned  ; /* The value returned by the interpreter. */

  linked_word  * parsed    ;
  unsigned int   end_line  ;
  unsigned int   hold_line ;

  /* Before we start, print out the copyright info, logo and
   * a guide to some helpful functions.
   */
  print_logo ();
  printf ("The FACT programming language interactive shell\n"
	  "(c) 2010, 2011 Matthew Plant, under the GPL version 3.\n");

  /* Set the initial line number to 1 and the file name to
   * "stdin".
   */
  end_line         = 1       ;
  scope->file_name = "stdin" ;

  for ( ; ; ) /* Heh, that looks like a spider. */
    {
      /* Set the line number to end_line, in case we missed
       * any while evaluating the last expression.
       */
      scope->line = end_line;

      /* Then, get raw input for an entire expression. */
      input = get_input (stdin, &end_line);

      /* We do two checks for EOF signals: once before
       * tokenizing, and once after. I am not completely
       * sure as to why this is the case, but I do 
       * remember at some point it didn't exit so I
       * added the second check.
       */
      if (input == NULL)
	break;

      /* Check for incompletions, and if there are any
       * skip to the next expression.
       */
      if (check_for_incompletions ("stdin", input))
	continue;

      /* Tokenize the input. */
      tokenized = get_words (input);

      if (tokenized == NULL)
	break;

      /* Convert the tokens to a list, and then set the
       * list. Setting the list does some stuff that
       * makes converting to polish notation a lot
       * easier. For more information, read the function
       * docs.
       */
      parsed = create_list (tokenized  ) ;
      parsed = set_list    (parsed, END) ;

      /* Because the value set_list is not guaranteed
       * to point to the beginning of the list, we
       * have to move the pointer backwards until it
       * is.
       */
      while (parsed->previous != NULL)
	parsed = parsed->previous;

      /* Check for any parsing errors. */
      hold_line = end_line;
      if (check_for_errors (parsed, 0, &hold_line, false))
	{
	  /* If there were any errors, print them. */
	  print_parsing_error ("stdin", hold_line);
	  continue;
	}

      /* Call rev_shunting_yard, the function that actually
       * converts our list from infix notation into polish
       * notation format. It's not actually a shunting
       * yard algorithm, but I call it that.
       */
      rev_shunting_yard (parsed);

      /* Like set_list, rev_shunting yard does not
       * guarantee that 'previous' is not NULL, so we have
       * to move parsed back.
       */
      while (parsed->previous != NULL)
	parsed = parsed->previous;

      /* Convert the list back to a char ** format. */
      set_link (parsed);
      bytecode = convert_link (parsed);

      /* Compile the string array to bytecode and then
       * reset the instruction pointer to zero.
       */
      compile_to_bytecode (bytecode);
      reset_ip ();

      /* Evaluate the expression. */
      returned = eval_expression (scope, make_word_list (bytecode, true));

      /* If there were errors, print them out. Otherwise,
       * print the value of the variable or the name of
       * the function returned.
       */
      if (returned.type == ERROR_TYPE)
	errorman_dump (returned.error);
      else if (returned.type == VAR_TYPE)
	printf ("Returned value: %s\n", mpc_get_str (returned.v_point->data));
      else
	printf ("Returned object [%s]\n", returned.f_point->name);

      /* Check to see if the value is to be returned, and
       * if so, exit.
       */
      if (returned.return_signal)
	break;
    }
  puts ("\nExiting...");
}
      
      
