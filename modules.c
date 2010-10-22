#include "modules.h"

typedef struct _LIB
{
  void *library;
  char *file_path;
  struct _LIB *next;
} lib_t;

static lib_t *root = NULL;

/*
  I am a little tired, so this may be a
  giant clusterfuck of awful. I dunno.

  Also, I have to add the using of
  an environmental variable as
  opposed to just using the direct,
  path.
*/

FACT_t
load_lib (func_t *scope, word_list args)
{
  int pos;
  
  char *fpath;
  char **parsed_input;

  void *checker;
  
  linked_word *formatted;
  
  FACT_t path;
  lib_t *scroller;

  struct element
  {
    char *name;
    char **arguments;

    void * (*function)(func_t *);
  };

  struct elmap
  {
    unsigned int size;
    struct element *elements;
  } MOD_MAP;

  path = eval (scope, args);

  if (!isvar_t (path))
    {
      if (iserror_t (path))
	return path;
      else
	return errorman_throw_reg (scope, "path to library to be loaded cannot be a function");
    }

  fpath = array_to_string (path.v_point);

  if (root == NULL)
    {
      root = better_malloc (sizeof (lib_t));
      scroller = root;
    }
  else
    {
      for (scroller = root; scroller->next != NULL; scroller = scroller->next)
	{
	  if (!strcmp (scroller->file_path, fpath))
	    return errorman_throw_reg (scope, "cannot re-load library");
	}
      scroller->next = better_malloc (sizeof (lib_t));
      scroller = scroller->next;
    }

  scroller->next = NULL;
  scroller->file_path = fpath;

  scroller->library = dlopen (fpath, RTLD_LAZY);

  if (scroller->library == NULL)
    {
      printf ("IMPORT ERROR: %s\n", dlerror ());
      return errorman_throw_reg (scope, combine_strs ("could not import module ",
						    array_to_string (path.v_point)));
    }

  checker = dlsym (scroller->library, "MOD_MAP");

  if (checker == NULL)
    return errorman_throw_reg (scope, "could not find MOD_MAP symbol in module");

  /* This is why I love C: */
  MOD_MAP = *((struct elmap *) checker);  

  for (pos = 0; pos < loaded->size; pos++)
    {
      func_t *ref;

      if ((ref = add_func (scope, loaded->elements[pos].name)) == NULL)
	continue; /* if it couldn't be added, just skip it. */

      ref->args = loaded->elements[pos].arguments;
      ref->extrn_func = loaded->elements[pos].function;
    }
  
  return FACT_get_ui (1);
}

void
close_libs ( void )
{
  lib_t *scroller;

  for (scroller = root; scroller != NULL; scroller = scroller->next)
    {
      if (scroller->library != NULL) /* in case there was an error loading something */
	dlclose (scroller->library);
    }
}
