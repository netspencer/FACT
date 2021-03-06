/* This file is part of FACT.
 *
 * FACT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FACT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FACT. If not, see <http://www.gnu.org/licenses/>.
 */

#include "FACT.h"

var_t *
get_local_var (func_t *scope, char *name)
{
  var_t *curr;

  for (curr = scope->vars; curr != NULL; curr = curr->next)
    {
      if (strcmp (curr->name, name) == 0)
	return curr;
    }

  return NULL;
}

func_t *
get_local_func (func_t *curr, char *name)
{
  for (curr = curr->funcs; curr != NULL; curr = curr->next)
    {
      if (strcmp (curr->name, name) == 0)
	return curr;
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
  var_t *new;

  /* I have been put under the impression that memory allocated
   * by GC Malloc is automatically initialized to 0/NULL/false/etc.
   * However, I play this memory game safe.
   */
  new = FACT_malloc (sizeof (var_t));
  new->array_up = NULL;
  new->next = NULL;
  new->name = NULL;
  new->locked = false ;
  mpc_init (&(new->data));
  mpz_init_set_ui (new->array_size, 1);

  return new;
}

func_t *
alloc_func ()
{
  func_t *new;

  /* See my comment for alloc_var (), it applies to this 
   * as well.
   */
  new = FACT_malloc (sizeof (func_t));
  new->line = 1;
  new->locked = false;
  new->lines = NULL;
  new->name = NULL;
  new->file_name = NULL;
  new->next = NULL;
  new->args = NULL;
  new->body = NULL;
  new->vars = NULL;
  new->extrn_func = NULL;
  new->funcs = NULL;
  new->up = NULL;
  new->caller = NULL;
  new->usr_data = NULL;
  new->array_up = NULL;
  new->variadic = NULL;
  
  mpz_init_set_ui (new->array_size, 1);

  return new;
}

void
free_var (var_t *dead_man)
{
  var_t *hold_up;
  var_t *hold_next;

  if (dead_man == NULL)              /* if no such man exists, do nothing */
    return;

  hold_up = dead_man->array_up;      /* get his contacts */
  hold_next = dead_man->next;

  mpz_clear (dead_man->data.object); /* destroy his data */
  FACT_free (dead_man);

  free_var (hold_up);                /* now do the same for all his contacts */
  free_var (hold_next);
}

void
free_func (func_t *dead_house)
{
  var_t  *hold_vars;
  func_t *hold_funcs;
  func_t *hold_next;
  func_t *hold_up;
  
  if (dead_house == NULL) /* if there is no house, leave */
    return;

  hold_funcs = dead_house->funcs;
  hold_next = dead_house->next;
  hold_up = dead_house->array_up;
  hold_vars = dead_house->vars;

  FACT_free (dead_house->body);
  FACT_free (dead_house->args);
  FACT_free (dead_house);

  free_func (hold_funcs);
  free_func (hold_next);
  free_func (hold_up);

  free_var (hold_vars);
}
  

var_t *
add_var (func_t *scope, char *name)
{
  var_t *scroller;
  var_t *hold_next;

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
  func_t *scroller;
  func_t *hold_next;

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
resize_var (var_t *resizable, mpz_t new_size)
{
  mpz_t i;
  var_t *scroller;

  scroller = resizable;

  //  if (resizable->array_size < new_size)
  if (mpz_cmp (resizable->array_size, new_size) < 0)
    {
      if (!mpz_cmp_ui (resizable->array_size, 1))
	{
	  scroller->array_up = alloc_var ();
	  scroller->array_up->name = scroller->name;
	}
      
      scroller = scroller->array_up;
    
      for (mpz_init_set_ui (i, 1); scroller->next != NULL;
	   scroller = scroller->next, mpz_add_ui (i, i, 1));

      while (mpz_cmp (i, new_size) < 0)
	{
	  scroller->next = alloc_var ();
	  scroller->next->name = scroller->name;
	  scroller = scroller->next;
	  mpz_add_ui (i, i, 1);
	}
    }
  else
    {
      if (!mpz_cmp_ui (new_size, 1))
	{
	  free_var (scroller->array_up);
	  scroller->array_up = NULL;
	}
      else
	{
	  scroller = scroller->array_up;
	  
	  for (mpz_init_set_ui (i, 1); mpz_cmp (i, new_size) < 0;
	       scroller = scroller->next, mpz_add_ui (i, i, 1));

	  free_var (scroller->next);
	  scroller->next = NULL;
	}
    }
  mpz_set (resizable->array_size, new_size);
  return resizable;
}
	
func_t *
resize_func (func_t *resizable, mpz_t new_size)
{
  mpz_t i;
  func_t *scroller;

  scroller = resizable;

  if (mpz_cmp (resizable->array_size, new_size) < 0)
    {
      if (!mpz_cmp_ui (resizable->array_size, 1))
	{
	  scroller->array_up = alloc_func ();
	  scroller->array_up->name = scroller->name;
	}
      
      scroller = scroller->array_up;

      for (mpz_init_set_ui (i, 1); scroller->next != NULL;
	   scroller = scroller->next, mpz_add_ui (i, i, 1));

      while (mpz_cmp (i, new_size) < 0)
	{
	  scroller->next = alloc_func ();
	  scroller->next->name = scroller->name;
	  scroller = scroller->next;
	  mpz_add_ui (i, i, 1);
	}
    }
  else
    {
      if (!mpz_cmp_ui (new_size, 1))
	{
	  free_func (scroller->array_up);
	  scroller->array_up = NULL;
	}
      else
	{
	  scroller = scroller->array_up;

	  for (mpz_init_set_ui (i, 1); mpz_cmp (i, new_size) < 0;
	       scroller = scroller->next, mpz_add_ui (i, i, 1));
	  
	  free_func (scroller->next);
	  scroller->next = NULL;
	}
    }
  
  mpz_set (resizable->array_size, new_size);
  return resizable;
}      
