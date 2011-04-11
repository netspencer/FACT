/* This file is part of FACT.
 *
 * FACT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FACT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FACT. If not, see <http://www.gnu.org/licenses/>.
 */

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
  int  i;
  int  p_count, b_count, c_count;
  int  curr_char;
  bool in_quotes;  
  char *input;

  if (fp == stdin)
    printf ("%s ", start_prompt);

  input = NULL;
  p_count = b_count = c_count = 0;
  in_quotes = false;
  
  for (i = 1; (curr_char = fgetc (fp)) != EOF; i++)
    {
      // Skip the char if it invalid
      if (curr_char > 126 || curr_char < 0)
        continue;
      
      if (curr_char == '#' && !in_quotes)
	{
	  while ((curr_char = fgetc (fp)) != EOF && curr_char != '\n')
	    ;
          i--;
          ungetc ('\n', fp);
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
      input = FACT_realloc (input, sizeof (char) * (i + 2));
      input[i] = ((curr_char != EOF) ? curr_char : '\0');
      input[i + 1] = '\0';
      for (i--; i >= 0; i--)
	{
	  if (!isspace ((int) input[i]) && input[i] != '\0' && input[i] != -1)
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

  int  *newlines;
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

  hold_input = NULL;
  scope->file_name = "stdin";
  scope->line = 1;
  end_line = 0;
  
  for (;;) // Heh, that looks like a spider.
    {
      /* Set the line number to end_line, in case we missed any while evaluating
       * the last expression.
       */
      scope->line += end_line;
      end_line = 0;

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

      // Get the newlines and parse the string.
      tokenized = parse (tokenized, "stdin", scope->line, &newlines);

      if (tokenized == NULL)
        continue;

      // Reset the instruction pointer.
      reset_ip ();

      // Evaluate the expression.
      returned = eval_expression (scope, make_syn_tree (tokenized, newlines));

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
