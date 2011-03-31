#include "FACT.h"

/**
 * execfile - run external FACT program files.
 * @TODO:
 *   + Needs to be fixed to handle if/else statements on the
 *     top level.
 *   + This cannot be done by allocating space for the entire
 *     file, that is simply not an option.
 */

FACT_t
run_file (func_t *scope, const char *f_name, bool silent)
{
  int    print_parsed;
  int    hold_line;
  FACT_t returned;
  unsigned int end_line;

  int  *newlines;
  char *hold_fn;
  char *input;
  char **parsed_input;
  FILE *fp;

  if (!silent)
    printf ("Opening file <%s>\n", f_name);

  fp               = fopen (f_name, "r");
  hold_line        = scope->line;
  scope->line      = 1;
  hold_fn          = scope->name;
  scope->name      = (char *) f_name;
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

      parsed_input = get_words (input);

      if (parsed_input == NULL)
        break;

      newlines = get_newlines (parsed_input);
      parsed_input = parse (parsed_input, f_name, end_line);

      if (parsed_input == NULL)
        break;

      reset_ip ();
      returned = eval_expression (scope, make_syn_tree (parsed_input, newlines));

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

