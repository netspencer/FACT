#include "modules.h"

typedef struct _LIB
{
  void *library;
  char *name;
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
  FACT_t path;
  lib_t *scroller;

  if (root == NULL)
    {
      root = better_malloc (sizeof (lib_t));
      scroller = root;
    }
  else
    {
      for (scroller = root; scroller->next != NULL; scroller = scroller->next)
	{
	  if (!strcmp (scroller->name, args.syntax[0]))
	    return errorman_throw_reg (scope, "cannot re-load library");
	}
      scroller->next = better_malloc (sizeof (lib_t));
      scroller = scroller->next;
    }

  scroller->next = NULL;
  scroller->name = args.syntax[0];

  args.move_forward[0] = true;

  path = eval (scope, args);

  if (!isvar_t (path))
    {
      if (iserror_t (path))
	return path;
      else
	return errorman_throw_reg (scope, "path to library to be loaded cannot be a function");
    }

  scroller->library = dlopen (array_to_string (path.v_point), RTLD_LAZY);

  if (scroller->library == NULL)
    return errorman_throw_reg (scope, combine_strs ("could not load library ",
						    array_to_string (path.v_point)));

  return FACT_get_ui (1);
}

FACT_t
call_lib (func_t *scope, word_list args)
{
  FACT_t prim_name;
  
  lib_t *scroller;

  void *function;

  for (scroller = root; scroller != NULL; scroller = scroller->next)
    {
      if (!strcmp (scroller->name, args.syntax[0]))
	break;
    }

  if (scroller == NULL)
    return errorman_throw_reg (scope, "call to unknown library");

  args.move_forward[0] = true;

  prim_name = eval (scope, args);

  if (!isvar_t (prim_name))
    {
      if (iserror_t (prim_name))
	return prim_name;
      else
	return errorman_throw_reg (scope, "primitive name cannot be a function");
    }

  function = dlsym (scroller->library, array_to_string (prim_name.v_point));

  while (args.move_forward[0])
    {
      args.syntax++;
      args.move_forward++;
    }

  /* This is a real stretch of a cast. */
  return ((FACT_t (*)(func_t *, word_list)) function) (scope, args);
}

void
close_libs ( void )
{
  lib_t *scroller;

  for (scroller = root; scroller != NULL; scroller = scroller->next)
    dlclose (scroller->library);
}
