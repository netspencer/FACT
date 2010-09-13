#include "management.h"

/* * * * * * * * * * * * * * * * * *
 * alloc_var: allocate the space   *
 * for a var struct and initialize *
 * all the values.                 *
 * * * * * * * * * * * * * * * * * */

var *get_local_var (func *scope, char *word)
{
  var *scroller;

  for (scroller = scope->vars; scroller != NULL; scroller = scroller->next)
    {
      if (strcmp (scroller->name, word) == 0)
	return scroller;
    }

  return NULL;
}

func *get_local_func (func *scope, char *word)
{
  func *scroller;

  for (scroller = scope->funcs; scroller != NULL; scroller = scroller->next)
    {
      if (strcmp (scroller->name, word) == 0)
	return scroller;
    }

  return NULL;
}

var *get_var (func *scope, char *word)
{
  while (scope != NULL)
    {
      if (get_local_var (scope, word) != NULL)
	return get_local_var (scope, word);

      scope = scope->up;
    }

  return NULL;
}

func *get_func (func *scope, char *word)
{
  while (scope != NULL)
    {
      if (get_local_func (scope, word))
	return get_local_func (scope, word);

      scope = scope->up;
    }

  return NULL;
}

var *alloc_var ()
{
  var *new;

  new = (var *) better_malloc (sizeof (var));
  new->array_size = 1;
  new->array_up = NULL;
  new->next = NULL;
  new->name = NULL;
  mpz_init (new->data);

  return new;
}

func *alloc_func ()
{
  func *new;

  new = (func *) better_malloc (sizeof (func));
  new->array_size = 1;
  new->array_up = NULL;
  new->up = NULL;
  new->next = NULL;
  new->args = NULL;
  new->body = NULL;
  new->vars = NULL;
  new->funcs = NULL;

  return new;
}

void free_var (var *dead_man)
{
  var *hold_up;
  var *hold_next;

  if (dead_man == NULL) /* if no such man exists, do nothing */
    return;

  hold_up = dead_man->array_up; /* get his contacts */
  hold_next = dead_man->next;

  mpz_clear (dead_man->data); /* destroy his data */
  GC_free (dead_man); /* and kill him */

  free_var (hold_up); /* now do the same for all his contacts */
  free_var (hold_next);
}

void free_func (func *dead_house)
{
  func *hold_funcs;
  func *hold_next;
  func *hold_up;
  var *hold_vars;
  
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
  

var *add_var (func *scope, char *name)
{
  var *scroller;
  var *hold_next;

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

func *addFunc (func *scope, char *name)
{
  func *scroller;
  func *hold_next;

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

var *resize_var (var *resizable, int new_size)
{
  int count;
  var *scroller;

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
	
func *resize_func (func *resizable, int new_size)
{
  int count;
  func *scroller;

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
