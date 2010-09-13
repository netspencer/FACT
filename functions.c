#include "interpreter.h"

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
			
 
a_type add_func (func *scope, char **words)
{
  a_type func;
  a_type error;
  char **args_formatted;
  char **block_formatted;
  int pos_args;
  int pos_block;
  int position;

  error.type = ERROR_TYPE;
  error.error.function = "add_func";

  func = eval (scope, words);

  if (func.type != FUNCTION_TYPE)
    {
      error.error.error_code = INVALPRIM;
      return error;
    }

  if (words[1] == NULL || strcmp (words[1], "("))
    {
      error.error.error_code = SYNTAX;
      return error;
    }

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
    {
      error.error.error_code = SYNTAX;
      return error;
    }

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

char **copy (char **words)
{
  int pos;
  int count;
  char **temp;

  if (words == NULL)
    return NULL;

  for (pos = 0; *(words + pos) != NULL; pos++);

  temp = (char **) better_malloc (sizeof (char *) * (pos + 1));

  for (count = 0; count < pos; count++)
    temp[count] = words[count];

  temp[count] =  NULL;

  return temp;
}

a_type prepare_function (func *scope, func *new_scope, char **words)
{
  a_type evald;
  a_type arg;
  a_type passed;
  a_type error;
  var *hold;
  var *temp;
  int pos;
  int count;
  char **args_stored;

  evald = eval (scope, words);

  error.type = ERROR_TYPE;
  error.error.function = "prepare_function";

  if (evald.type != FUNCTION_TYPE
      || evald.f_point->args == NULL)
    {
      error.error.error_code = INVALPRIM;
      return error;
    }

  words++;
  args_stored = copy (evald.f_point->args);
  new_scope->up = evald.f_point->up;

  for (pos = 0; *(args_stored + pos) != NULL; pos++)
    {
      arg = eval (new_scope, args_stored + pos);

      if (*(words + pos) == NULL)
	{
	  error.error.error_code = LESSFUNC;
	  return error;
	}

      passed = eval (scope, words + pos);

      if (passed.type != arg.type || arg.type == ERROR_TYPE)
	{
	  error.error.error_code = INVALFUNC;
	  return error;
	}

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
	  arg.f_point->vars = passed.f_point->vars;
	  arg.f_point->funcs = passed.f_point->funcs;
	  arg.f_point->array_up = passed.f_point->array_up;
	  arg.f_point->up = passed.f_point->up;
	}

      if (args_stored[++pos] == NULL)
        {
          if (*(words + pos) != NULL && strcmp (*(words + pos), ",") == 0)
	    {
	      error.error.error_code = MOREFUNC;
	      return error;
	    }
	  
	  break;
        }
      else if (strcmp (*(args_stored + pos), ",") == 0)
	{
	  if (strcmp (*(words + pos),",") != 0)
	    {
	      error.error.error_code = LESSFUNC;
	      return error;
	    }
	}
	
      else
        {
	  printf ("%s\n", args_stored[pos]);
	  error.error.error_code = SYNTAX;
	  return error;
        }
    }

  for (count = pos, pos = -1; words[pos + count] != NULL; pos++)
    words[pos] = words[pos + count];
  for (pos++; pos < count; pos++)
    words[pos] = NULL;
  
  return evald;
}

a_type new_scope (func *scope, char **words)
{
  a_type prepared;
  a_type return_value;
  func *new_scope;
  char **copy_body;

  new_scope = alloc_func ();
  new_scope->name = "lambda";
  
  prepared = prepare_function (scope, new_scope, words);

  if (prepared.type == ERROR_TYPE) 
    return prepared;

  copy_body = copy (prepared.f_point->body + 1);
  prepared = procedure (new_scope, copy_body);

  if (prepared.type == ERROR_TYPE)
    {
      prepared.error.scope = new_scope;
      return prepared;
    }

  return_value.f_point = new_scope;
  return_value.type = FUNCTION_TYPE;

  return return_value;
}

a_type run_func (func *scope, char **words)
{
  a_type return_value;
  a_type prepared;
  func *new_scope;
  char **copied_body;

  new_scope = alloc_func ();
  new_scope->name = "lambda";

  prepared = prepare_function (scope, new_scope, words);

  if (prepared.type == ERROR_TYPE)
    {
      prepared.error.scope = new_scope;
      return prepared;
    }
  
  copied_body = copy (prepared.f_point->body);

  return_value = expression (new_scope, copied_body);

  return_value.isret = false;

  return return_value;
}


a_type in_scope (func *scope, char **words)
{
  a_type new_scope;
  a_type error;
  a_type to_return;
  int pos;

  error.type = ERROR_TYPE;
  error.error.function = "in_scope";

  new_scope = eval (scope, words);

  if (new_scope.type != FUNCTION_TYPE)
    {
      error.error.scope = scope;
      error.error.error_code = INVALPRIM;
      return error;
    }

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
