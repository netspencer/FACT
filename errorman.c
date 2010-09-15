#include "errorman.h"

const char *errors [] = {
    "no error",
    "cannot assign variable to function",
    "cannot assign function to variable",
    "redefinition of variable",
    "redefinition of function",
    "too few arguments to primitive",
    "too few arguments to function",
    "too many arguments to primitive",
    "too many arguments to function",
    "invalid arguments to primitive",
    "invalid arguments to function",
    "array out of bounds",
    "divide by zero error",
    "modulo by zero error",
    "syntax error"
};

void
errorman_dump (err error, int line_num, const char *filename)
{
  printf ("Error in <%s> on line %d", filename, line_num);

  if (error.scope != NULL)
    printf (", function [%s]", error.scope->name);

  printf (": %s\n",  errors[error.error_code]);
}
/*
struct error {
  char *scope_name;
  char *error;
  bool is_dumped;
  struct error *next;
} root_node = {
    NULL,
    NULL,
    true,
    NULL
};

void errorman_add (func *scope, char *error_details) Will be support for (...) later
{
  struct error *navigator = &root_node;

  while (navigator->is_dumped != true)
    {
      if (navigator->next == NULL)
        {
          navigator->next = (struct error *) better_malloc (sizeof (struct error));
          navigator->scope_name = NULL;
          navigator->error = NULL;
          navigator->next->is_dumped = true;
          navigator->next->next = NULL;
        }
      navigator = navigator->next;
    }
  navigator->scope_name = (scope == NULL) ? "unknown" : scope->name;
  navigator->error = error_details;
  navigator->is_dumped = false;
}

void errorman_dump ()
{
  struct error *navigator = &root_node;

  while (navigator != NULL && navigator->is_dumped != true)
    {
      printf("Error in function [%s]: %s\n", navigator->scope_name, navigator->error);
      navigator->is_dumped = true;
      navigator = navigator->next;
    }
}
*/
