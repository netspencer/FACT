#include "modules.h"

typedef struct _LIB
{
  void        * library   ;
  char        * file_path ;
  struct _LIB * next      ;
} lib_t;

static lib_t * root;

/* I am a little tired, so this may be a giant clusterfuck
 * of awful. I don't know. Also, I have to add the using of
 * an environmental variable as opposed to just using the
 * direct path.
 */

FACT_t
load_lib (func_t *scope)
{
  int      index    ;
  char   * fpath    ;
  lib_t  * scroller ;
  var_t  * path     ;
  struct   elements
  {
    char *   name;
    char *   arguments;
    void *(* function)(func_t *);
  }      *   MOD_MAP;

  path  = get_var (scope, "path"); 
  scope = scope->up;
  fpath = array_to_string (path);

  if (root == NULL)
    {
      root     = better_malloc (sizeof (lib_t));
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
      scroller       = scroller->next;
    }

  scroller->next      = NULL;
  scroller->file_path = fpath;
  scroller->library   = dlopen (fpath, RTLD_LAZY);

  if (scroller->library == NULL)
    {
      printf ("IMPORT ERROR: %s\n", dlerror ());
      return errorman_throw_reg (scope, combine_strs ("could not import module ",
						      array_to_string (path)));
    }

  MOD_MAP = dlsym (scroller->library, "MOD_MAP");

  if (MOD_MAP == NULL)
    return errorman_throw_reg (scope, "could not find MOD_MAP symbol in module");

  for (index = 0; MOD_MAP[index].name != NULL; index++)
    {
      func_t * ref;

      if ((ref = add_func (scope, MOD_MAP[index].name)) == NULL)
	continue; /* if it couldn't be added, just skip it. */

      if (MOD_MAP[index].arguments == NULL)
	continue;
      ref->args = get_words (combine_strs (MOD_MAP[index].arguments, " "));

      ref->locked = true;
      ref->extrn_func = MOD_MAP[index].function;
    }
  
  return FACT_get_ui (0);
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
