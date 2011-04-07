#include "FACT.h"

FACT_t
run_file (func_t *scope, const char *f_name, bool silent)
{
  FACT_t returned;
  unsigned int end_line;
  unsigned long hold_line;

  int  *newlines;
  char *hold_fn;
  char *input;
  char *hold_input;
  char **tokenized;
  char **hold_tokens;
  FILE *fp;

  if (!silent)
    printf ("Opening file <%s>\n", f_name);

  fp = fopen (f_name, "r");
  hold_line = scope->line;
  scope->line = 1;
  hold_fn = scope->name;
  scope->name = (char *) f_name;
  scope->file_name = (char *) f_name;

  if (fp == NULL)
    {
      scope->name = hold_fn;
      FACT_ret_error (scope, "could not open file");
    }

  end_line = 1;
  for (;;)
    {
      scope->line = end_line;
      input = get_input (fp, &end_line, NULL, NULL);

      if (input == NULL)
        break;

      tokenized = get_words (input);
      if (tokenized == NULL)
        break;

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
	      hold_input = get_input (fp, &end_line, NULL, NULL);

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
	  tokenized = get_words (input);
	}

      tokenized = parse (tokenized, f_name, end_line, &newlines);

      if (tokenized == NULL)
        break;

      reset_ip ();
      returned = eval_expression (scope, make_syn_tree (tokenized, newlines));

      if (returned.type == ERROR_TYPE)
        {
          errorman_dump (returned.error);
          continue;
        }

      if (returned.return_signal == true)
        break;
    }
  
  fclose (fp);
  if (!silent)
    printf ("Closing file <%s>.\n", f_name);

  scope->name = hold_fn;
  scope->line = hold_line;
  return returned;
}

