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
  int count;
  int paren_count;
  int bracket_count;
  int curly_count;
  int c;

  bool in_quotes;
  
  char *input;
  //  char *temp;

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
	  count--;
	  (*line_number)++;
	}

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
      else if (c == '"')
	in_quotes = !in_quotes;

      if (c == ';' || c == '}')
	{
	  input[count] = '\0';

	  /*
	  in_quotes = 0;
	  paren_count = 0;
	  bracket_count = 0;
	  curly_count = 0;
	  while (*temp != '\0')
	    {
	      if (*temp == '(')
		paren_count++;
	      else if (*temp == ')')
	paren_count--;
	      else if (*temp == '[')
		bracket_count++;
	      else if (*temp == ']')
		bracket_count--;
	      else if (*temp == '{')
		curly_count++;
	      else if (*temp == '}')
		curly_count--;
	      else if (*temp == '"')
		in_quotes = !in_quotes;
	      temp++;
	    }
	  */

	  if (paren_count == 0 && bracket_count == 0 && curly_count == 0 && !in_quotes
	      && (input[count - 1] == ';' || input[count - 1] == '}'))
	    break;
	  
	}
      
    }
  
  if (input != NULL)
    {
      input = better_realloc (input, sizeof (char) * (count + 2));
      input[count] = c;
      input[count + 1] = '\0';
    }

  return input;
}

void
shell (func *main_scope)
{
  char *input;
  char **parsed_input;
  unsigned int line_num;
  //int read;

  a_type returned;

#if PARSING >= 2
  linked_word *formatted;
#endif

  /* Print out the disclaimer and logo. */
  printf ("The FACT programming language interactive shell\n(c) 2010 Matthew Plant, under a copyleft license.\n");
  print_logo ();

  line_num = 1;

  for (;;)
    {
      input = get_input (stdin, &line_num);

      if (input == NULL)
        {
          printf ("\nExiting...\n");
	  return;
        }
      
      parsed_input = get_words (input);
#if PARSING >= 2
      if (parsed_input == NULL)
	{
	  printf ("\nExiting...\n");
	  return;
	}
      
      formatted = create_list (parsed_input);

      formatted = set_list (formatted, END);

      while (formatted->previous != NULL)
	formatted = formatted->previous;

      rev_shunting_yard (formatted);

      while (formatted->previous != NULL)
	formatted = formatted->previous;

      set_link (formatted);
      parsed_input = convert_link (formatted);


      /*
      for (read = 0; parsed_input[read] != NULL; read++)
	printf ("%s ", parsed_input[read]);
      putchar ('\n');
      */

#endif

#if PARSING < 2
      rev_shunting_yard (parsed_input, '\0');
#endif
      returned = expression (main_scope, parsed_input);

      if (returned.type == ERROR_TYPE)
        {
          errorman_dump (returned.error, line_num, "stdin");
          continue;
        }

      if (returned.type == VAR_TYPE)
	printf ("Returned value: %s\n", mpc_get_str (returned.v_point->data));
        /*gmp_printf ("Returned value: %Zd\n", returned.v_point->data);*/
      else
        printf ("Returned object [%s]\n", returned.f_point->name);

      if (returned.isret == true)
        {
          printf ("Exiting...\n");
	  return;
        }
    }
}


/*
void shell () 
{
  char *input;
  char **toSend;
  a_type temp;
  linked_word *prepared;
  int size = 1;
  int not_needed;
  func main_scope = {
    "main",
    NULL,
    NULL,
    0,
    NULL,
    NULL,
    NULL,
    NULL
  };

  printf ("The FACT programming language interactive shell\n(c) 2010 Matthew Plant, under a copyleft license.\n");
  print_logo ();

  while (true)
    {
      input = get_input ("\0", stdin);

      if (input == NULL)
        {
          printf ("\nExiting\n");
	  free_var (main_scope.vars);
	  free_func (main_scope.funcs);
          return;
        }

      toSend = getWords (input);

#ifdef TESTING

      prepared = create_links (toSend, SEMI);

      
      toSend = convert_link (prepared);
      
      for (not_needed = 0; toSend[not_needed] != NULL; not_needed++)
        printf ("%s ", toSend[not_needed]);
      putchar ('\n');

      rev_shunting_yard (prepared);

      toSend = NULL;

      while (prepared->previous != NULL)
	prepared = prepared->previous;

      set_link (prepared);

    
      while (prepared->previous != NULL)
	prepared = prepared->previous; 

      toSend = convert_link (prepared);
      
      for (not_needed = 0; toSend[not_needed] != NULL; not_needed++)
        printf ("%s ", toSend[not_needed]);
      putchar ('\n');
#endif

#ifndef TESTING
      rev_shunting_yard (toSend, '\0');
#endif
      
      temp = expression (&main_scope, toSend);

      if (temp.type == ERROR_TYPE)
        {
          errorman_dump (temp.error);
          continue;
        }

      if (temp.type == VAR_TYPE)
        gmp_printf ("Returned value: %Zd\n", temp.v_point->data);
      else
        printf ("Returned object [%s]\n", temp.f_point->name);

      if (temp.isret == true)
        {
          printf ("Exiting\n");
	  free_var (main_scope.vars);
	  free_func (main_scope.funcs);
          return;
        }

    }
}
*/
