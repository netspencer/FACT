#include "FACT.h"

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
get_input (FILE *fp, unsigned int *line_number, const char *start_prompt, const char *cont_prompt)
{
  int    i;
  int    p_count;
  int    b_count;
  int    c_count;
  int    curr_char;
  bool   in_quotes;  
  char * input;

  if (fp == stdin)
    printf ("%s ", start_prompt);

  /* Doing var = var = x; is unclear and annoying. */
  input     = NULL;
  p_count   = 0;
  b_count   = 0;
  c_count   = 0;
  in_quotes = false;
  
  for (i = 1; (curr_char = fgetc (fp)) != EOF; i++)
    {
      if (curr_char == '#' && !in_quotes)
	{
	  while ((curr_char = fgetc (fp)) != EOF && curr_char != '\n')
	    ;
	  ungetc (curr_char, fp);
	  i--;
	  continue;
	}
      if (curr_char == '\n')
	{
	  ++*line_number;
	  if (i > 1 && fp == stdin
	      && ((in_quotes || p_count || b_count || c_count)
		  || (input[i - 2] != ';' || input[i - 2] != '}')))
	    printf ("%s ", cont_prompt);
	}

      input = better_realloc (input, (i + 1) * sizeof (char));
      input[i - 1] = curr_char;

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
	      if (input[i - 2] != '\\'
		  || (i > 3 && input[i - 3] == '\\'))
		in_quotes = false;
	    }
	  else
	    in_quotes = true;
	}
      if (curr_char == ';' || curr_char == '}')
	{
	  input[i] = '\0';

	  if (p_count == 0 && b_count == 0 && c_count == 0 && !in_quotes)
	    break; 
	}      
    }
  
  if (input != NULL)
    {
      input        = better_realloc (input, sizeof (char) * (i + 2));
      input[i]     = curr_char;
      input[i + 1] = '\0';
      for (i--; i >= 0; i--)
	{
	  if (!isspace ((int) input[i]) && input[i] != '\0')
	    return input;
	}  
    }
  return NULL;
}

void
shell (func_t * scope)
{
  /**
   * shell - when FACT is run by default it comes to here. The
   * main user interface for FACT, this function grabs an
   * expression of input, parses it, runs it, and repeats. It
   * will continue to do so until the user does a C-d, C-c,
   * returns a value from the main scope, or calls the exit
   * function.
   *
   * @scope  - the scope to use when evaluating expressions.
   */
  FACT_t       returned;  // The value returned by the interpreter.
  unsigned int end_line;  // ...
  unsigned int hold_line; // ...
  
  char *input;
  char *hold_input;   // Used in the main loop to check for else clauses.
  char **tokenized;   // input, tokenized.
  char **hold_tokens; // Also used to check for else clauses.

  /* Before we start, print out the copyright info, logo and
   * a guide to some helpful functions.
   */
  print_logo ();
  printf ("The FACT programming language interactive shell\n"
	  "© 2010, 2011 Matthew Plant, under the GPL version 3.\n");

  /* Set the initial line number to 1 and the file name to
   * "stdin". Also, set hold_input to NULL.
   */
  end_line         = 1;
  hold_input       = NULL;
  scope->file_name = "stdin";

  for (;;) // Heh, that looks like a spider.
    {
      /* Set the line number to end_line, in case we missed any while evaluating
       * the last expression.
       */
      scope->line = end_line;

      // Then, get raw input for an entire expression.
      if (hold_input == NULL)
	input = get_input (stdin, &end_line, "S>", "C>");
      else
	{
	  input = hold_input;
	  hold_input = NULL;
	}

      /* We do two checks for EOF signals: once before tokenizing, and once after.
       * I am not completely sure as to why this is the case, but I do remember at
       * some point it didn't exit so I added the second check.
       */
      if (input == NULL)
	break;

      // Tokenize the input.
      tokenized = get_words (input);
      if (tokenized == NULL)
	break;
      
      /* If the first token in the expression is if/on_error, continue to get input
       * as long as the first token is else. I could forsee this being an issue in
       * places where the else is placed erroneosly, but that'll be fixed later I
       * assume.
       */ 
      if ((tokenized[0][0] == '\n'
	   && (!tokcmp (tokenized[1], "if")
	       || !tokcmp (tokenized[1], "error")))
	  || (!tokcmp (tokenized[0], "if")
	      || !tokcmp (tokenized[0], "error")))
	{
	  for (;;)
	    {
	      /* Go through all the steps we went through from the start of the loop
               * down to here.
	       */
	      hold_input = get_input (stdin, &end_line, "?>", "C>");

	      if (hold_input == NULL|| (hold_tokens = get_words (hold_input)) == NULL)
		break;
	      // Check to see if the statement starts with else.
	      if ((hold_tokens[0][0] == '\n'
		   && !tokcmp (hold_tokens[1], "else"))
		  || !tokcmp (hold_tokens[0], "else"))
		{
		  input[strlen (input) - 1] = '\0';
		  input = combine_strs (input, hold_input);
		  hold_input = NULL;
		}
	      else
		break;
	    }
          printf ("\n");
	  tokenized = get_words (input);
	}

      // Parse the string.
      tokenized = parse (tokenized);
      // TODO: skip if NULL, as it indicates errors.

      // Reset the instruction pointer.
      reset_ip ();

      // Evaluate the expression.
      returned = eval_expression (scope, make_word_list (tokenized, false));

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
      
      
