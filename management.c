#include "management.h"

/* * * * * * * * * * * * * * * * * *
 * alloc_var: allocate the space   *
 * for a var struct and initialize *
 * all the values.                 *
 * * * * * * * * * * * * * * * * * */

var_t *
get_local_var (func_t *scope, char *word)
{
  var_t * scroller;

  for (scroller = scope->vars; scroller != NULL; scroller = scroller->next)
    {
      if (strcmp (scroller->name, word) == 0)
	return scroller;
    }

  return NULL;
}

func_t *
get_local_func (func_t *scope, char *word)
{
  func_t * scroller;

  for (scroller = scope->funcs; scroller != NULL; scroller = scroller->next)
    {
      if (strcmp (scroller->name, word) == 0)
	return scroller;
    }

  return NULL;
}

var_t *
get_var (func_t *scope, char *word)
{
  while (scope != NULL)
    {
      if (get_local_var (scope, word) != NULL)
	return get_local_var (scope, word);

      scope = scope->up;
    }

  return NULL;
}

func_t *
get_func (func_t *scope, char *word)
{
  while (scope != NULL)
    {
      if (get_local_func (scope, word))
	return get_local_func (scope, word);

      scope = scope->up;
    }

  return NULL;
}

var_t *
alloc_var ()
{
  var_t * new;

  new = (var_t *) better_malloc (sizeof (var_t));
  new->array_size = 1;
  new->array_up = NULL;
  new->next = NULL;
  new->name = NULL;
  mpc_init (&(new->data));

  return new;
}

func_t *
alloc_func ()
{
  func_t * new;

  new = (func_t *) better_malloc (sizeof (func_t));
  new->array_size = 1;
  new->array_up = NULL;
  new->name = NULL;
  new->extrn_func = NULL;
  new->up = NULL;
  new->next = NULL;
  new->args = NULL;
  new->body = NULL;
  new->vars = NULL;
  new->funcs = NULL;

  return new;
}

void
free_var (var_t *dead_man)
{
  var_t * hold_up;
  var_t * hold_next;

  if (dead_man == NULL)              /* if no such man exists, do nothing */
    return;
  
  hold_up = dead_man->array_up;      /* get his contacts */
  hold_next = dead_man->next;
  mpz_clear (dead_man->data.object); /* destroy his data */
  free_var (hold_up);                /* now do the same for all his contacts */
  free_var (hold_next);
}

void
free_func (func_t *dead_house)
{
  var_t  * hold_vars;
  func_t * hold_funcs;
  func_t * hold_next;
  func_t * hold_up;
  
  if (dead_house == NULL) /* if there is no house, leave */
    return;

  hold_funcs = dead_house->funcs;
  hold_next = dead_house->next;
  hold_up = dead_house->array_up;
  hold_vars = dead_house->vars;

  GC_free (dead_house->body);
  GC_free (dead_house->args);
  GC_free (dead_house);

  free_func (hold_funcs);
  free_func (hold_next);
  free_func (hold_up);

  free_var (hold_vars);
}
  

var_t *
add_var (func_t *scope, char *name)
{
  var_t * scroller;
  var_t * hold_next;

  if (scope->vars == NULL)
    {
      scope->vars = alloc_var ();
      scope->vars->name = name;

      return scope->vars;
    }  
  if (strcmp (scope->vars->name, name) == 0)
    {
      hold_next = scope->vars->next;
      scope->vars->next = NULL;
      free_var (scope->vars);

      scope->vars = alloc_var ();
      scope->vars->name = name;
      scope->vars->next = hold_next;

      return scope->vars;
    }  
  for (scroller = scope->vars; scroller->next != NULL; scroller = scroller->next)
    {
      if (strcmp (scroller->next->name, name) == 0)
	{
	  hold_next = scroller->next->next;

	  scroller->next->next = NULL;
	  scroller->next = alloc_var ();
	  scroller->next->name = name;
	  scroller->next->next = hold_next;

	  return scroller->next;
	}
    }
  
  scroller->next = alloc_var ();
  scroller->next->name = name;

  return scroller->next;
}

func_t *
add_func (func_t *scope, char *name)
{
  func_t * scroller;
  func_t * hold_next;

  if (scope->funcs == NULL)
    {
      scope->funcs = alloc_func ();
      scope->funcs->name = name;
      scope->funcs->up = scope;

      return scope->funcs;
    }
  if (strcmp (scope->funcs->name, name) == 0)
    {
      hold_next = scope->funcs->next;
      scope->funcs->next = NULL;
      free_func (scope->funcs);

      scope->funcs = alloc_func ();
      scope->funcs->name = name;
      scope->funcs->up = scope;
      scope->funcs->next = hold_next;

      return scope->funcs;
    }  
  for (scroller = scope->funcs; scroller->next != NULL; scroller = scroller->next)
    {
      if (strcmp (scroller->next->name, name) == 0)
	{
	  hold_next = scroller->next->next;

	  scroller->next->next = NULL;
	  scroller->next = alloc_func ();
	  scroller->next->name = name;
	  scroller->next->up = scope;
	  scroller->next->next = hold_next;

	  return scroller->next;
	}
    }
  
  scroller->next = alloc_func ();
  scroller->next->name = name;
  scroller->next->up = scope;

  return scroller->next;
}

var_t *
resize_var (var_t *resizable, int new_size)
{
  int     count;
  var_t * scroller;

  scroller = resizable;

  if (resizable->array_size < new_size)
    {
      if (resizable->array_size == 1)
	{
	  scroller->array_up = alloc_var ();
	  scroller->array_up->name = scroller->name;
	}
      
      scroller = scroller->array_up;
      
      for (count = 1; scroller->next != NULL;
	   scroller = scroller->next, count++);

      while (count < new_size)
	{
	  scroller->next = alloc_var ();
	  scroller->next->name = scroller->name;
	  scroller = scroller->next;
	  count++;
	}
    }
  else
    {
      if (new_size == 1)
	{
	  free_var (scroller->array_up);
	  scroller->array_up = NULL;
	  
	}
      else
	{
	  scroller = scroller->array_up;
	  
	  for (count = 1; count < new_size;
	       scroller = scroller->next, count++);

	  free_var (scroller->next);
	  scroller->next = NULL;
	}
    }
  resizable->array_size = new_size;
  return resizable;
}
	
func_t *
resize_func (func_t *resizable, int new_size)
{
  int      count;
  func_t * scroller;

  scroller = resizable;

  if (resizable->array_size < new_size)
    {
      if (resizable->array_size == 1)
	{
	  scroller->array_up = alloc_func ();
	  scroller->array_up->name = scroller->name;
	}
      
      scroller = scroller->array_up;
      
      for (count = 1; scroller->next != NULL;
	   scroller = scroller->next, count++);

      while (count < new_size)
	{
	  scroller->next = alloc_func ();
	  scroller->next->name = scroller->name;
	  scroller = scroller->next;
	  count++;
	}
    }
  else
    {
      if (new_size == 1)
	{
	  free_func (scroller->array_up);
	  scroller->array_up = NULL;
	}

      scroller = scroller->array_up;

      for (count = 1; count < new_size;
	   scroller = scroller->next, count++);

      free_func (scroller->next);
      scroller->next = NULL;
    }

  resizable->array_size = new_size;

  return resizable;
}      
