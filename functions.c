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
			
 
FACT_t
liven_func (func_t *scope, word_list expression)
{
  int pos_args;
  int pos_block;
  int position;

  char **args_formatted;
  char **block_formatted;

  FACT_t func;
  
  func = eval (scope, expression);

  if (func.type == ERROR_TYPE)
    return func;
  
  if (func.type != FUNCTION_TYPE)
    return errorman_throw_reg (scope, "cannot give body to non-function");

  while (expression.move_forward[0])
    {
      expression.syntax++;
      expression.move_forward++;
    }

  if (expression.syntax[0] == NULL || strcmp (expression.syntax[0], "("))
    return errorman_throw_reg (scope, "expected '(' after function");

  pos_args = get_exp_length (expression.syntax + 1, ')');
  
  args_formatted = (char **) better_malloc ((sizeof (char *)) * pos_args);
  
  args_formatted[pos_args - 1] = NULL;
  position = pos_args - 1;
    
  while (position > 0)
    {
      position--;
      args_formatted[position] = expression.syntax[position + 1];
      expression.move_forward[position + 1] = true;
    }
    
  if (expression.syntax[pos_args] == NULL)
    return errorman_throw_reg (scope, "no body given");

  pos_block = get_exp_length_first (expression.syntax + pos_args, ';');
  block_formatted = better_malloc ((sizeof (char *)) * pos_block);

  block_formatted[--pos_block] = NULL;
  position = pos_block;

  while (position > 0)
    {
      position--;
      block_formatted[position] = expression.syntax[position + pos_args + 1];
      expression.move_forward[position + pos_args + 1] = true;
    }
  
  func.f_point->args = args_formatted;
  func.f_point->body = block_formatted;
  func.f_point->up = scope;

  for (position = 0; position < pos_args + pos_block; position++)
    expression.move_forward[position] = true;
  
  return func;
}

int
count_until_NULL (char **words)
{
  int pos;

  for (pos = 0; words[pos] != NULL; pos++)
    ;

  return pos;
}

FACT_t
prepare_function (func_t *scope, func_t *new_scope, word_list expression)
{
  int pos;
  int count;

  FACT_t evald;
  FACT_t arg;
  FACT_t passed;

  var_t *hold;
  var_t *temp;

  word_list arg_list;

  extern void set_array (bool *, int);

  evald = eval (scope, expression);

  if (evald.type == ERROR_TYPE)
    return evald;

  if (evald.type != FUNCTION_TYPE)
    return errorman_throw_reg (scope, "cannot run a non-function");

  if (evald.f_point->args == NULL)
    return errorman_throw_reg (scope, "given function has no body");

  while (expression.move_forward[0])
    {
      expression.syntax++;
      expression.move_forward++;
    }
  
  arg_list.syntax = copy (evald.f_point->args);
  arg_list.move_forward = better_malloc (sizeof (int) *
					 ((count = count_until_NULL (arg_list.syntax)) + 1));
  //set_array (arg_list.move_forward, count);

  new_scope->up = evald.f_point->up;
  new_scope->name = evald.f_point->name;

  for (pos = 0; arg_list.syntax[0] != NULL; pos++)
    {
      arg = eval (new_scope, arg_list);

      if (!strcmp (expression.syntax[0], "<-"))
	return errorman_throw_reg (scope, "expected more arguments");

      passed = eval (scope, expression);

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
	  temp = clone_var_t (passed.v_point, arg.v_point->name);
	  passed.v_point->next = hold;
	  arg.v_point->array_up = temp->array_up;
	  mpc_set (&(arg.v_point->data), temp->data);
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

      while (arg_list.move_forward[0])
	{
	  arg_list.syntax++;
	  arg_list.move_forward++;
	}

      while (expression.move_forward[0])
	{
	  expression.syntax++;
	  expression.move_forward++;
	}
	
      if (arg_list.syntax[0] == NULL)
        {
          if (strcmp (expression.syntax[0], "<-") && strcmp (expression.syntax[0], ",") == 0)
	    return errorman_throw_reg (scope, "expected fewer arguments");
	  
	  break;
        }
      else if (strcmp (arg_list.syntax[0], ",") == 0)
	{
	  if (strcmp (expression.syntax[0],",") != 0)
	    return errorman_throw_reg (scope, "expected more arguments");

	  arg_list.move_forward[0] = true; 
	  expression.move_forward[0] = true;
	}
      else
	return errorman_throw_reg (scope, "syntax error in argument declarations");
    }

  expression.move_forward[0] = true;

  /*
  for (count = pos + 2, pos = -1; words[pos + count] != NULL; pos++)
    words[pos] = words[pos + count];

  for (pos++; pos < count; pos++)
    words[pos] = NULL;
  */
  
  return evald;
}

FACT_t
new_scope (func_t *scope, word_list expression)
{
  char **copy_body;
  
  FACT_t prepared;
  FACT_t return_value;

  func_t *new_scope;

  new_scope = alloc_func ();
  
  prepared = prepare_function (scope, new_scope, expression);

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

FACT_t
run_func (func_t *scope, word_list expression_list)
{
  char **copied_body;

  FACT_t return_value;
  FACT_t prepared;

  func_t *new_scope;

  new_scope = alloc_func ();

  prepared = prepare_function (scope, new_scope, expression_list);

  if (prepared.type == ERROR_TYPE)
    return prepared; 
  
  copied_body = copy (prepared.f_point->body);

  return_value = expression (new_scope, copied_body);

  return_value.isret = false;
  return_value.break_signal = false;

  return return_value;
}

FACT_t
in_scope (func_t *scope, word_list expression)
{
  FACT_t new_scope;
  FACT_t to_return;

  new_scope = eval (scope, expression);

  if (new_scope.type != FUNCTION_TYPE)
    return errorman_throw_reg (scope, "the argument to : must be a function");
  
  to_return = eval (new_scope.f_point, expression);

  return to_return;
}

FACT_t
lambda (func_t *scope, word_list expression)
{
  FACT_t return_value;

  return_value.type = FUNCTION_TYPE;

  return_value.f_point = alloc_func ();
  return_value.f_point->name = "lambda";

  return return_value;
}
