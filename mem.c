#include "common.h"

a_type
defunc_array (func *base, func *scope, word_list expression)
{
  int size;
  int pos;

  /* char **temp; */

  a_type return_value;
  a_type array_size;
  a_type checker;

  func *scroller;

  extern void set_array (bool *, int);
  extern int count_until_NULL (char **);
  
  return_value.f_point = alloc_func ();

  if (expression.syntax[0] == NULL)
    return errorman_throw_reg (scope, "cannot make an anonymous function array");
  
  return_value.type = FUNCTION_TYPE;

  if (base == NULL)
    base = alloc_func ();

  if (strcmp (expression.syntax[0], "["))
    {
      expression.move_forward[0] = true;
      base->name = expression.syntax[0];
    }
  else
    {
      expression.syntax++;
      expression.move_forward++;
      
      array_size = get_array_size (scope, expression);

      while (expression.move_forward[0])
	{
	  expression.syntax++;
	  expression.move_forward++;
	}

      if (array_size.type == ERROR_TYPE)
	return array_size;
      
      if (array_size.type != VAR_TYPE)
	return errorman_throw_reg (scope, "array size needs to be a variable");

      if (((size = (mpc_get_si (array_size.v_point->data))) > 1000 || size < 1))
	return errorman_throw_reg (scope, "array size give is invalid");

      base = resize_func (base, size);

      for (scroller = base->array_up, pos = 0; pos < size; pos++, scroller = scroller->next)
	{
	  set_array (expression.move_forward, count_until_NULL (expression.syntax));
	  checker = defunc_array (scroller, scope, expression);
	  
	  if (checker.type == ERROR_TYPE)
	    return checker;
	}
	
      base->name = base->array_up->name;
    }

  return_value.f_point = base;

  return return_value;
}     

a_type
defunc (func *scope, word_list expression)
{
  a_type return_value;
  a_type temp;

  return_value.f_point = alloc_func ();

  if (expression.syntax[0] == NULL)
    return errorman_throw_reg (scope, "cannot define anonymous functions");

  return_value.type = FUNCTION_TYPE;

  if (!strcmp (expression.syntax[0], "["))
    {
      temp = defunc_array (NULL, scope, expression);

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
      return_value.f_point = add_func (scope, expression.syntax[0]);

      if (return_value.f_point == NULL)
	return errorman_throw_reg (scope, "could not define function");

      return_value.f_point->up = scope;
      return_value.f_point->array_size = 1;
    }

  expression.move_forward[0] = true;
  
  return return_value;
}

a_type
def_array (var *base, func *scope, word_list expression)
{
  int size;
  int pos;

  a_type return_value;
  a_type array_size;
  a_type checker;

  var *scroller;

  extern void set_array (bool *, int);
  extern int count_until_NULL (char **);

  return_value.v_point = alloc_var ();

  if (expression.syntax[0] == NULL)
    return errorman_throw_reg (scope, "cannot define anonymous array");
  
  return_value.type = VAR_TYPE;

  if (base == NULL)
    base = alloc_var ();

  if (strcmp (expression.syntax[0], "["))
    {
      expression.move_forward[0] = true;
      base->name = expression.syntax[0];
    }
  else
    {
      expression.syntax++;
      expression.move_forward++;

      array_size = get_array_size (scope, expression);

      while (expression.move_forward[0])
	{
	  expression.syntax++;
	  expression.move_forward++;
	}

      if (array_size.type == ERROR_TYPE)
	return array_size;

      if (array_size.type != VAR_TYPE)
	return errorman_throw_reg (scope, "array size needs to be a variable");
      
      if ((size = (mpc_get_si (array_size.v_point->data))) > 1000 || size < 2)
	return errorman_throw_reg (scope, "invalid array size");
      
      base = resize_array (base, size);

      for (scroller = base->array_up, pos = 0; pos < size; pos++, scroller = scroller->next)
	{
	  set_array (expression.move_forward, count_until_NULL (expression.syntax));
	  checker = def_array (scroller, scope, expression);
	  
	  if (checker.type == ERROR_TYPE)
	    return checker;
	}
      
      base->name = base->array_up->name;
    }

  return_value.v_point = base;

  return return_value;
}     

a_type
define (func *scope, word_list expression)
{
  a_type return_value;
  a_type temp;

  return_value.v_point = alloc_var ();

  if (expression.syntax[0] == NULL)
    return errorman_throw_reg (scope, "cannot define anonymous var");
    
  return_value.type = VAR_TYPE;

  if (!strcmp (expression.syntax[0], "["))
    {
      temp = def_array (NULL, scope, expression);

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
    return_value.v_point = add_var (scope, expression.syntax[0]);

  expression.move_forward[0] = true;

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

  mpc_set (&(clone->data), surrogate->data);

  return clone;
}

a_type
set (func *scope, word_list expression)
{
  a_type arg1;
  a_type arg2;

  var *hold;
  var *copy;

  arg1 = eval (scope, expression);
  arg2 = eval (scope, expression);

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
      mpc_set (&(arg1.v_point->data), copy->data);
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

  /*
  for (pos = 0; words[pos + 1] != NULL; pos++)
    words[pos] = words[pos + 1];
  
  words[pos] = NULL;
  */
  
  return arg1;
}

a_type
get_array_size (func *scope, word_list expression)
{
  int pos;
  
  a_type return_value;

  pos = get_exp_length (expression.syntax, ']');

  if (pos == 0)
    return errorman_throw_reg (scope, "cannot evaluate empty brackets");
  else if (expression.syntax[pos - 1][0] != ']')
    return errorman_throw_reg (scope, "syntax error; expected closing ']'");

  expression.syntax[pos - 1] = NULL;

  return_value = eval (scope, expression);

  expression.syntax[pos - 1] = "]";
  expression.move_forward[pos - 1] = true;

  return return_value;
}

a_type
return_array (func *scope, word_list expression)
{
  int size;

  a_type return_value;
  a_type hold;

  type_define type;

  union
  {
    var  *var_root;
    func *func_root;
  } roots;

  union
  {
    var  *var_value;
    func *func_value;
  } values;
 
  if (!strcmp (expression.syntax[0], "]"))
    return errorman_throw_reg (scope, "expected body before ']'");

  if ((hold = eval (scope, expression)).type == ERROR_TYPE)
    return hold;

  type = hold.type;

  if (type == VAR_TYPE)
    {
      values.var_value = hold.v_point;
     
      roots.var_root = alloc_var ();
      roots.var_root->name = "result";
      roots.var_root->array_up = values.var_value;
    }
  else if (type == FUNCTION_TYPE)    
    {
      values.func_value = hold.f_point;
     
      roots.func_root = alloc_func ();
      roots.func_root->name = "result";
      roots.func_root->array_up = values.func_value;
    }
  
  for (size = 1; size <= 1000; size++)
    {
      while (expression.move_forward[0])
	{
	  expression.syntax++;
	  expression.move_forward++;
	}

      if (!strcmp (expression.syntax[0], "]"))
	break;
      else if (strcmp (expression.syntax[0], ","))
	return errorman_throw_reg (scope, "expected ',' or closing ']'");
      else while (expression.move_forward[0])
	     {
	       expression.syntax++;
	       expression.move_forward++;
	     }

      hold = eval (scope, expression);

      if (hold.type == ERROR_TYPE)
	return hold;
      
      if (type != hold.type)
	{
	  if (type == VAR_TYPE)
	    return errorman_throw_reg (scope, "unexpected function returned in a variable array");
	  else
	    return errorman_throw_reg (scope, "unexpected variable returned in a function array");
	}

      if (type == VAR_TYPE)
	{
	  values.var_value->next = hold.v_point;
	  values.var_value = values.var_value->next;
	}
      else if (type == FUNCTION_TYPE)
	{
	  values.func_value->next = hold.f_point;
	  values.func_value = values.func_value->next;
	}
    }

  return_value.type = type;

  if (type == VAR_TYPE)
    {
      if (size == 1)
	return_value.v_point = roots.var_root->array_up;
      else
	return_value.v_point = roots.var_root;

      return_value.v_point->array_size = size;
    }
  else if (type == FUNCTION_TYPE)
    {
      if (size == 1)
	return_value.f_point = roots.func_root->array_up;
      else
	return_value.f_point = roots.func_root;

      return_value.f_point->array_size = size;
    }
  // return errorman_throw_reg (scope, "experimental");

  return return_value;
}

a_type
size_of (func *scope, word_list expression)
{
  a_type return_value;
  a_type evald;
  
  if (expression.syntax[0] == NULL)
    return errorman_throw_reg (scope, "cannot find the sizeof nothing");

  evald = eval (scope, expression);

  if (evald.type == ERROR_TYPE)
    return evald;

  return_value.v_point = alloc_var ();
  return_value.type = VAR_TYPE;

  if (evald.type == VAR_TYPE)
    mpc_set_si (&(return_value.v_point->data), evald.v_point->array_size);
  else
    mpc_set_si (&(return_value.v_point->data), evald.f_point->array_size);

  return return_value;
}

a_type
get_array_var (var *root, func *scope, word_list expression)
{
  int size;
  int position;

  a_type return_value;
  a_type array_size;

  return_value.type = VAR_TYPE;

  
  while (expression.move_forward[0])
    {
      expression.syntax++;
      expression.move_forward++;
    }
  
  if (expression.syntax[0] == NULL || strcmp (expression.syntax[0], "["))
    {
      return_value.v_point = root;
      return return_value;
    }
  
  expression.move_forward[0] = true;
  
  expression.syntax++;
  expression.move_forward++;

  array_size = get_array_size (scope, expression);

  if (array_size.type == ERROR_TYPE)
    return array_size;
  
  if (array_size.type != VAR_TYPE)
    return errorman_throw_reg (scope, "array position cannot be a function");

  size = mpc_get_si (array_size.v_point->data);

  if (size >= root->array_size || size < 0)
    return errorman_throw_reg (scope, "array out of bounds");

  if (root->array_size == 1)
    {
      while (expression.move_forward[0])
	{
	  expression.syntax++;
	  expression.move_forward++;
	}
      
      if (expression.syntax[0] != NULL && !strcmp (expression.syntax[0], "["))
	return errorman_throw_reg (scope, "array out of bounds");

      return_value.v_point = root;

      return return_value;
    }

  for (root = root->array_up, position = 0; position < size; position++)
    root = root->next;

  return_value = get_array_var (root, scope, expression);

  return return_value;
}

a_type
get_array_func (func *root, func *scope, word_list expression)
{
  int size;
  int position;

  a_type return_value;
  a_type array_size;

  return_value.type = FUNCTION_TYPE;

  while (expression.move_forward[0])
    {
      expression.syntax++;
      expression.move_forward++;
    }

  if (expression.syntax[0] == NULL || strcmp (expression.syntax[0], "["))
    {
      return_value.f_point = root;
      return return_value;
    }

  expression.move_forward[0] = true;

  expression.syntax++;
  expression.move_forward++;

  array_size = get_array_size (scope, expression);

  if (array_size.type == ERROR_TYPE)
    return array_size;
  
  if (array_size.type != VAR_TYPE)
    return errorman_throw_reg (scope, "array position cannot be a function");

  size = mpc_get_si (array_size.v_point->data);

  if (size >= root->array_size || size < 0)
    return errorman_throw_reg (scope, "array out of bounds");

  if (root->array_size == 1)
    {
      while (expression.move_forward[0])
	{
	  expression.syntax++;
	  expression.move_forward++;
	}

      if (expression.syntax[1] != NULL && !strcmp (expression.syntax[1], "["))
	return errorman_throw_reg (scope, "array out of bounds");

      return_value.f_point = root;
       
      return return_value;
    }

  for (root = root->array_up,
	 position = 0; position < size; position++)
    root = root->next;

  return_value = get_array_func (root, scope, expression);

  return return_value;
}
