#include "interpreter.h"

a_type
defunc_array (func *base, func *scope, char **words)
{
  a_type return_value;
  a_type array_size;
  a_type checker;
  int size;
  int pos;
  char **temp;
  func *scroller;

  return_value.f_point = alloc_func ();

  if (words[0] == NULL)
    return errorman_throw_reg (scope, "cannot make an anonymous function array");
  
  return_value.type = FUNCTION_TYPE;

  if (base == NULL)
    base = alloc_func ();

  if (strcmp (words[0], "[") != 0)
    {
      base->name = words[0];
    }
  else
    {
      array_size = get_array_size (scope, words + 1);

      if (array_size.type == ERROR_TYPE)
	return array_size;
      
      if (array_size.type != VAR_TYPE)
	return errorman_throw_reg (scope, "array size needs to be a variable");

      if (((size = (mpz_get_si (array_size.v_point->data))) > 1000 || size < 1))
	return errorman_throw_reg (scope, "array size give is invalid");

      base = resize_func (base, size);

      for (scroller = base->array_up, pos = 0; pos < size; pos++, scroller = scroller->next)
	{
	  temp = copy (words + 1);
	  checker = defunc_array (scroller, scope, temp);
	  
	  if (checker.type == ERROR_TYPE)
	    return checker;
	}
	
      base->name = base->array_up->name;
    }

  return_value.f_point = base;

  return return_value;
}     

a_type
defunc (func *scope, char **words)
{
  a_type return_value;
  a_type temp;
  int pos;

  return_value.f_point = alloc_func ();
  
  if (words[0] == NULL)
    return errorman_throw_reg (scope, "cannot define anonymous functions");

  return_value.type = FUNCTION_TYPE;

  if (strcmp (words[0], "[") == 0)
    {
      temp = defunc_array (NULL, scope, words);

      if (temp.type != ERROR_TYPE)
	{
	  return_value.f_point = add_func (scope, temp.f_point->array_up->name);
	  return_value.f_point->array_up = temp.f_point->array_up;
	  return_value.f_point->up = scope;
	  return_value.f_point->array_size = temp.f_point->array_size;
	  GC_free (temp.f_point);
	}
      else
	return_value = temp;
    }
  else
    {
      return_value.f_point = add_func (scope, words[0]);

      if (return_value.f_point == NULL)
	return errorman_throw_reg (scope, "could not define function");

      return_value.f_point->up = scope;
      return_value.f_point->array_size = 1;
    }

  for (pos = 0; words[pos] != NULL; pos++)
    words[pos] = words[pos + 1];
  
  return return_value;
}

a_type
def_array (var *base, func *scope, char **words)
{
  a_type return_value;
  a_type array_size;
  a_type checker;
  int size;
  int pos;
  char **temp;
  var *scroller;

  return_value.v_point = alloc_var ();

  if (words[0] == NULL)
    return errorman_throw_reg (scope, "cannot define anonymous array");
  
  return_value.type = VAR_TYPE;

  if (base == NULL)
    base = alloc_var ();

  if (strcmp (words[0], "[") != 0)
    base->name = words[0];
  else
    {
      array_size = get_array_size (scope, words + 1);

      if (array_size.type == ERROR_TYPE)
	return array_size;

      if (array_size.type != VAR_TYPE)
	return errorman_throw_reg (scope, "array size needs to be a variable");
      
      if ((size = (mpz_get_si (array_size.v_point->data))) > 1000 || size < 2)
	return errorman_throw_reg (scope, "invalid array size");
      
      base = resize_array (base, size);

      for (scroller = base->array_up, pos = 0; pos < size; pos++, scroller = scroller->next)
	{
	  temp = copy (words + 1);
	  checker = def_array (scroller, scope, temp);
	  
	  if (checker.type == ERROR_TYPE)
	    return checker;
	}
      base->name = base->array_up->name;
    }

  return_value.v_point = base;

  return return_value;
}     

a_type
define (func *scope, char **words)
{
  a_type return_value;
  a_type temp;
  int pos;

  return_value.v_point = alloc_var ();

  if (words[0] == NULL)
    return errorman_throw_reg (scope, "cannot define anonymous var");
    
  return_value.type = VAR_TYPE;

  if (strcmp (words[0], "[") == 0)
    {
      temp = def_array (NULL, scope, words);

      if (temp.type != ERROR_TYPE)
	{
	  return_value.v_point = add_var (scope, temp.v_point->array_up->name);
	  return_value.v_point->array_up = temp.v_point->array_up;
	  return_value.v_point->array_size = temp.v_point->array_size;
	  GC_free (temp.v_point);
	}
      else
	return_value = temp;
    }
  else
    {
      return_value.v_point = add_var (scope, words[0]);

      for (pos = 0; words[pos] != NULL; pos++)
        words[pos] = words[pos + 1];
    }

  return return_value;
}

var *
clone_var (var *surrogate, char *name)
{
  var *clone;

  if (surrogate == NULL)
    return NULL;

  clone = alloc_var ();
  clone->name = name;
  clone->array_size = surrogate->array_size;

  clone->next = clone_var (surrogate->next, name);
  clone->array_up = clone_var (surrogate->array_up, name);

  mpz_set (clone->data, surrogate->data);

  return clone;
}

a_type
set (func *scope, char **words)
{
  a_type arg1;
  a_type arg2;
  var *hold;
  var *copy;
  int pos;

  arg1 = eval (scope, words);
  arg2 = eval (scope, words + 1);

  if (arg1.type == ERROR_TYPE || arg2.type == ERROR_TYPE)
    return (arg1.type == ERROR_TYPE) ? arg1 : arg2;

  
  if (arg1.type == VAR_TYPE)
    {
      if (arg2.type == FUNCTION_TYPE)
	return errorman_throw_reg (scope, "cannot set a var to a function");
      
      hold = arg2.v_point->next;
      arg2.v_point->next = NULL;
      copy = clone_var (arg2.v_point, arg1.v_point->name);
      arg2.v_point->next = hold;
      free_var (arg1.v_point->array_up);
      arg1.v_point->array_up = copy->array_up;
      mpz_set (arg1.v_point->data, copy->data);
      arg1.v_point->array_size = copy->array_size;
    }
  else if (arg1.type == FUNCTION_TYPE)
    {
      if (arg2.type == VAR_TYPE)
	return errorman_throw_reg (scope, "cannot set a function to a var");

      arg1.f_point->array_size = arg2.f_point->array_size;
      arg1.f_point->args = arg2.f_point->args;
      arg1.f_point->body = arg2.f_point->body;
      arg1.f_point->vars = arg2.f_point->vars;
      arg1.f_point->funcs = arg2.f_point->funcs;
      arg1.f_point->array_up = arg2.f_point->array_up;
      arg1.f_point->up = arg2.f_point->up;
    }

  for (pos = 0; words[pos + 1] != NULL; pos++)
    words[pos] = words[pos + 1];
  
  words[pos] = NULL;
  
  return arg1;
}

a_type
get_array_size (func *scope, char **words) /* this REALLY needs to get fixed (in terms of syntax. It's a mess) */
{
  int pos;
  int count;
  char **formatted;
  
  pos = get_exp_length (words, ']');

  if (pos == 0)
    return errorman_throw_reg (scope, "cannot evaluate empty brackets");

  formatted = (char **) better_malloc ((sizeof (char *)) * pos);

  count = pos;

  formatted[--pos] = NULL;

  while (pos > 0)
    {
      pos--;
      formatted[pos] = words[pos];
    }

  for (pos = 0; words[count] != NULL; pos++, count++)
    words[pos] = words[count];
    
  words[pos] = words[count];

  return eval (scope, formatted);
}

/* deprecated as FUCK */
a_type
return_array (func *scope, char **words)
{
  a_type return_value;
  int count;
  var *var_scroll_through;
  func *func_scroll_through;
  a_type evald_pos;
  a_type evald_var;
 
  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();
  
  if (words[0] == NULL)
    return errorman_throw_reg (scope, "FUCK YOU");

  evald_pos = get_array_size (scope, words);
  count = mpz_get_si (evald_pos.v_point->data);

  if (evald_pos.type == ERROR_TYPE)
    return evald_pos;
  
  if (evald_pos.type != VAR_TYPE)
    return errorman_throw_reg (scope, "EEEEEEERRREER");

  evald_var = eval (scope, words);

  if (evald_var.type == ERROR_TYPE)
    return evald_var;
  else if (evald_var.type == VAR_TYPE)
    {
      if (count >= evald_var.v_point->array_size
	  || count < 0)
	return errorman_throw_reg (scope, "array out of bounds");

      if (evald_var.v_point->array_size == 1)
	return_value.v_point = evald_var.v_point;
      else
	{
	  for (var_scroll_through = evald_var.v_point->array_up;
	       count > 0; count--)
	    var_scroll_through = var_scroll_through->next;

	  return_value.v_point = var_scroll_through;
	}
      
      return_value.type = VAR_TYPE;
    }
  else
    {
      if (count >= evald_var.f_point->array_size
	  || count < 0)
	return errorman_throw_reg (scope, "array out of bounds");

      if (evald_var.f_point->array_size == 1)
	return_value.f_point = evald_var.f_point;
      else
	{
	  for (func_scroll_through = evald_var.f_point->array_up;
	       count > 0; count--)
	    func_scroll_through = func_scroll_through->next;

	  return_value.f_point = func_scroll_through;
	}
      return_value.type = FUNCTION_TYPE;
    }
  
   for (count = 0; words[count + 1] != NULL; count++)
     words[count] = words[count + 1];
      
  return return_value;
}

a_type
size_of (func *scope, char **words)
{
  a_type return_value;
  a_type evald;
  
  if (words[0] == NULL)
    return errorman_throw_reg (scope, "cannot find the sizeof nothing");

  evald = eval (scope, words);

  if (evald.type == ERROR_TYPE)
    return evald;

  return_value.v_point = alloc_var ();
  return_value.type = VAR_TYPE;

  mpz_set_si (return_value.v_point->data, evald.v_point->array_size);

  return return_value;
}


/* 
a_type
get_array (var *base, func *scope, char **words)
{
  a_type return_value;
  a_type array_size;
  int size;
  int pos;
  var *scroller;

  if (words[0] == NULL)
    {
      error.error.error_code = INVALPRIM;
      return error;
    }
  
  return_value.type = VAR_TYPE;
  return_value.v_point = alloc_var ();

  if (strcmp (words[0], "[") == 0)
    {
      array_size = get_array_size (scope, words + 1);

      if (array_size.type != VAR_TYPE)
	{
	  error.error.error_code = INVALPRIM;
	  return error;
	}

      if (((size = (mpz_get_si (array_size.v_point->data))) >= base->array_size)
	  || size < 0)
	{
	  error.error.error_code = ESCAPARR;
	  return error;
	}
     
      for (scroller = base->array_up, pos = 0; pos < size; pos++, scroller = scroller->next);

      return_value = get_array (base->array_up, scope, words + 1);

      return return_value;
    }

  return_value.v_point = base;

  return return_value;
}
*/

a_type
get_array_var (var *root, func *scope, char **words)
{
  a_type return_value;
  a_type array_size;
  int size;
  int position;

  return_value.type = VAR_TYPE;
  
  if (words[0] == NULL || strcmp (words[0], "["))
    {
      return_value.v_point = root;
      return return_value;
    }

  array_size = get_array_size (scope, words + 1);

  if (array_size.type == ERROR_TYPE)
    return array_size;
  
  if (array_size.type != VAR_TYPE)
    return errorman_throw_reg (scope, "array position cannot be a function");

  size = mpz_get_si (array_size.v_point->data);

  if (size >= root->array_size || size < 0)
    return errorman_throw_reg (scope, "array out of bounds");

  if (root->array_size == 1)
    {
      if (words[1] != NULL && !strcmp (words[1], "["))
	return errorman_throw_reg (scope, "array out of bounds");

      return_value.v_point = root;
      return return_value;
    }

  for (root = root->array_up,
	 position = 0; position < size; position++)
    root = root->next;

  return_value = get_array_var (root, scope, words + 1);

  for (position = 0; words [position] != NULL; position++)
    words[position] = words[position + 1];

  return return_value;
}

a_type
get_array_func (func *root, func *scope, char **words)
{
  a_type return_value;
  a_type array_size;
  int size;
  int position;

  return_value.type = FUNCTION_TYPE;
  
  if (words[0] == NULL || strcmp (words[0], "["))
    {
      return_value.f_point = root;
      return return_value;
    }

  array_size = get_array_size (scope, words + 1);

  if (array_size.type == ERROR_TYPE)
    return array_size;
  
  if (array_size.type != VAR_TYPE)
    return errorman_throw_reg (scope, "array position cannot be a function");

  size = mpz_get_si (array_size.v_point->data);

  if (size >= root->array_size || size < 0)
    return errorman_throw_reg (scope, "array out of bounds");

  if (root->array_size == 1)
    {
      if (words[1] != NULL && !strcmp (words[1], "["))
	return errorman_throw_reg (scope, "array out of bounds");

      return_value.f_point = root;
      return return_value;
    }

  for (root = root->array_up,
	 position = 0; position < size; position++)
    root = root->next;

  return_value = get_array_func (root, scope, words + 1);

  for (position = 0; words [position] != NULL; position++)
    words[position] = words[position + 1];

  return return_value;
}
