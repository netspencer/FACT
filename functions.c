#include "common.h"

/* * * * * * * * * * * * * * * * * * * * *
 * add_func:                             *
 *  - called by: '@'                     *
 *  - purpose: takes a function and      *
 *             adds the arguments and    *
 *             code block specified by   *
 *             the user.                 *
 *  - example:                           *
 *             @defunc example (def arg) *
 *               {                       *
 *                  return arg;          *
 *               }                       *
 *                                       *
 *             This sets example's args  *
 *             to "def arg" and its body *
 *             to "{ return arg }"       *
 *                                       *
 * * * * * * * * * * * * * * * * * * * * */
			
 
a_type
liven_func (func *scope, char **words)
{
  a_type func;
  char **args_formatted;
  char **block_formatted;
  int pos_args;
  int pos_block;
  int position;

  func = eval (scope, words);

  if (func.type == ERROR_TYPE)
    return func;
  
  if (func.type != FUNCTION_TYPE)
    return errorman_throw_reg (scope, "cannot give body to non-function");

  if (words[1] == NULL || strcmp (words[1], "("))
    return errorman_throw_reg (scope, "expected '(' after function");

  pos_args = get_exp_length (words + 2, ')');
  
  args_formatted = (char **) better_malloc ((sizeof (char *)) * pos_args);
  
  args_formatted[pos_args - 1] = NULL;
  position = pos_args - 1;
    
  while (position > 0)
    {
      position--;
      args_formatted[position] = words[position + 2];
    }
    
  if (words[pos_args] == NULL)
    return errorman_throw_reg (scope, "no body given");

  pos_block = get_exp_length (words + pos_args + 1, ';');
  block_formatted = (char **) better_malloc ((sizeof (char *)) * pos_block);

  block_formatted[--pos_block] = NULL;
  position = pos_block;

  while (position > 0)
    {
      position--;
      block_formatted[position] = words[position + pos_args + 2];
    }
  
  func.f_point->args = args_formatted;
  func.f_point->body = block_formatted;
  func.f_point->up = scope;

  return func;
}

a_type
prepare_function (func *scope, func *new_scope, char **words)
{
  a_type evald;
  a_type arg;
  a_type passed;
  var *hold;
  var *temp;
  int pos;
  int count;
  char **args_stored;

  evald = eval (scope, words);

  if (evald.type == ERROR_TYPE)
    return evald;

  if (evald.type != FUNCTION_TYPE)
    return errorman_throw_reg (scope, "cannot run a non-function");

  if (evald.f_point->args == NULL)
    return errorman_throw_reg (scope, "given function has no body");

  words++;
  args_stored = copy (evald.f_point->args);
  new_scope->up = evald.f_point->up;
  new_scope->name = evald.f_point->name;

  for (pos = 0; args_stored[pos] != NULL; pos++)
    {
      arg = eval (new_scope, args_stored + pos);

      if (!strcmp (words[pos], "<-"))
	return errorman_throw_reg (scope, "expected more arguments");

      passed = eval (scope, words + pos);

      if (arg.type == ERROR_TYPE)
	return arg;
      if (passed.type == ERROR_TYPE)
	return passed;
      
      if (passed.type != arg.type)
	return errorman_throw_reg (scope, "expected argument type does not match passed argument type");

      if (arg.type == VAR_TYPE)
	{
	  hold = passed.v_point->next;
	  passed.v_point->next = NULL;
	  temp = clone_var (passed.v_point, arg.v_point->name);
	  passed.v_point->next = hold;
	  arg.v_point->array_up = temp->array_up;
	  mpz_set (arg.v_point->data, temp->data);
	  arg.v_point->array_size = temp->array_size;
	}
      else if (arg.type == FUNCTION_TYPE)
	{
	  arg.f_point->array_size = passed.f_point->array_size;
	  arg.f_point->args = passed.f_point->args;
	  arg.f_point->body = passed.f_point->body;
	  arg.f_point->vars = passed.f_point->vars;
	  arg.f_point->funcs = passed.f_point->funcs;
	  arg.f_point->array_up = passed.f_point->array_up;
	  arg.f_point->up = passed.f_point->up;
	}

      if (args_stored[++pos] == NULL)
        {
          if (strcmp (words[pos], "<-") && strcmp (words[pos], ",") == 0)
	    return errorman_throw_reg (scope, "expected fewer arguments");
	  
	  break;
        }
      else if (strcmp (args_stored[pos], ",") == 0)
	{
	  if (strcmp (words[pos],",") != 0)
	    return errorman_throw_reg (scope, "expected more arguments");
	}
      else
	return errorman_throw_reg (scope, "syntax error in argument declarations");
    }

  //printf ("name = %s, pos = %d, words[pos] = %s\n", new_scope->name, pos, words[pos]);
  for (count = pos + 2, pos = -1; words[pos + count] != NULL; pos++)
    words[pos] = words[pos + count];
  //printf ("pos = %d, count = %d\n", pos, count);
  for (pos++; pos < count; pos++)
    words[pos] = NULL;
  
  return evald;
}

a_type
new_scope (func *scope, char **words)
{
  a_type prepared;
  a_type return_value;
  func *new_scope;
  char **copy_body;

  new_scope = alloc_func ();
  //  new_scope->name = scope->name;
  
  prepared = prepare_function (scope, new_scope, words);

  if (prepared.type == ERROR_TYPE) 
    return prepared; /* ha ha, that makes me chortle */

  copy_body = copy (prepared.f_point->body + 1);
  prepared = procedure (new_scope, copy_body);

  if (prepared.type == ERROR_TYPE)
    return prepared;

  return_value.f_point = new_scope;
  return_value.type = FUNCTION_TYPE;
  return_value.isret = false;
  return_value.break_signal = false;

  return return_value;
}

a_type
run_func (func *scope, char **words)
{
  a_type return_value;
  a_type prepared;
  func *new_scope;
  char **copied_body;

  new_scope = alloc_func ();
  //  new_scope->name = scope->name;

  prepared = prepare_function (scope, new_scope, words);

  if (prepared.type == ERROR_TYPE)
    return prepared; 
  
  copied_body = copy (prepared.f_point->body);

  return_value = expression (new_scope, copied_body);

  return_value.isret = false;
  return_value.break_signal = false;

  return return_value;
}


a_type
in_scope (func *scope, char **words)
{
  a_type new_scope;
  a_type to_return;
  int pos;

  new_scope = eval (scope, words);

  if (new_scope.type != FUNCTION_TYPE)
    return errorman_throw_reg (scope, "the argument to : must be a function");
  
  to_return = eval (new_scope.f_point, words + 1);

  for (pos = 0; words[pos] != NULL; pos++)
    words[pos] = words[pos + 2];

  return to_return;
}

/*
static char **copy_to_paren (char **to_copy)
{
  int length;
  char **return_value;

  length = get_exp_length (to_copy + 1);

  if (to_copy[length] == NULL)
    return NULL;
    
  return_value = (char **) better_malloc (sizeof (char *) length);

  return_value[length] = NULL;

  while (length > 0)
    {
      length--;
      return_value[length] = to_copy;
    }

  return return_value;
}
*/
