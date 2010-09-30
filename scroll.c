#include "management.h"

void scroll_array (var *to_look)
{
  int until;

  for (until = to_look->array_size, to_look = to_look->array_up; to_look != NULL && until > 0; until--)
    {
      printf ("->[%s:%d:", to_look->name, to_look->array_size);
      gmp_printf ("%Zd][", to_look->data.object);
      scroll_array (to_look);
      printf ("]");
      to_look = to_look->next;
    }
}

void scroll (func *scope)
{
  var *var_scroll;
  func *func_scroll;
  int pos;

  printf (".____________.\n"
          "| Variables: |\n"
          ".------------.\n");

  for (var_scroll = scope->vars; var_scroll != NULL; var_scroll = var_scroll->next)
    {
      printf ("[%s:%d:", var_scroll->name, var_scroll->array_size);
      fflush (stdout);
      gmp_printf ("%Zd]", var_scroll->data.object);
      fflush (stdout);
      scroll_array (var_scroll);
      printf ("\n\t|\n\tV\n");
      fflush (stdout);
    }

  printf("\tEND\n");
  fflush (stdout);
  printf (".____________.\n"
          "| Functions: |\n"
          ".------------.\n");

  for (func_scroll = scope->funcs; func_scroll != NULL; func_scroll = func_scroll->next)
    {
      printf ("[%s:%s]\n( ", func_scroll->name, (func_scroll->up != NULL) ? func_scroll->up->name : "(NULL)");
      fflush (stdout);
      for (pos = 0; func_scroll->args != NULL && *(func_scroll->args + pos) != NULL; pos++)
	{
	  printf ("%s ", *(func_scroll->args + pos));
	  fflush (stdout);
	}
      printf (")\n{ ");
      fflush (stdout);
      for (pos = 0; func_scroll->body != NULL &&  *(func_scroll->body + pos) != NULL; pos++)
	{
	  printf ("%s ", *(func_scroll->body + pos));
	  fflush (stdout);
	}
      printf ("}\n||\nVV\n");
      fflush (stdout);
      scroll (func_scroll);
      printf ("\t|\n\tV\n");
      fflush (stdout);
    }
  printf("\tEND\n");
  fflush (stdout);
}
