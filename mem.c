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

FACT_t
defunc_array (func_t *base, func_t *scope, syn_tree_t expression)
{
  /**
   * defunc_array - create a function array. This function is called by defunc
   * in order to create mult-dimensional arrays. The method used is recursive
   * traversing of the linked-list. Since def_array works in essentially the
   * same way, def_array isn't as commented.
   */
  mpz_t  i;
  mpz_t  size;
  FACT_t checker;
  FACT_t array_size;
  FACT_t return_value;
  unsigned long ip_mv;
  unsigned long ip;
  func_t *curr;

  // Set the return value defaults.
  return_value.f_point = alloc_func ();
  return_value.type = FUNCTION_TYPE;

  if (expression.syntax[0] == NULL)
    FACT_throw (scope, "cannot make an anonymous function array", expression);

  // If this is the first dimension, allocate the base. 
  if (base == NULL)
    base = alloc_func ();

  /* If the current token is not a [, that means we've finally
   * hit the name, and we should return.
   */
  if (tokcmp (expression.syntax[0], "["))
    {
      ip_mv = 1;
      base->name = expression.syntax[0];
      base->file_name = scope->file_name;
    }
  else
    {
      /* Since we know that the current token is a [, we can move the
       * ip and the expression one forward. Then, we get the array
       * size.
       */
      expression.syntax++;
      ip = get_ip ();
      array_size = get_array_size (scope, expression);

      /* Move the expression forward only by the difference in positions
       * between the last token evaluated by get_array_size and the
       * current token.
       */
      ip_mv = get_ip () - ip;
      expression.syntax += get_ip () - ip;

      /* If there was an error in the dimension declaration, we return
       * it. If get_array_size returned a function (get_array_size
       * works essentially the same way as parenthesis) then we
       * return an error.
       */
      if (array_size.type == ERROR_TYPE)
	return array_size;
      if (array_size.type != VAR_TYPE)
	FACT_throw (scope, "array size needs to be a variable", expression);

      // Set array_size to an mpc_t and truncate it.
      mpz_init (size);
      mpc_get_mpz (size, array_size.v_point->data);

      // If the array size is out of bounds, return an error.
      if (mpz_cmp_ui (size, 1) <= 0)
	FACT_throw (scope, "invalid array size", expression);

      base = resize_func_array (base, size);
      curr = base->array_up;
      for (mpz_init (i);  mpz_cmp (i, size) < 0; mpz_add_ui (i, i, 1))
	{
	  /* Reset the ip if it has moved at all. This is probably the
	   * case if there were dimensions beyond this one. Do not do
	   * This for the last loop, as we want it to carry over.
	   */
	  set_ip (ip);
	  
	  checker = defunc_array (curr, scope, expression);
	  curr = curr->next;

	  /* Check to see if defunc_array returned an error. */
	  if (checker.type == ERROR_TYPE)
	    return checker;
	}
      /* Get the name and file name from the dimension above. */
      base->name = base->array_up->name;
      base->file_name = base->array_up->file_name;
    }
  return_value.f_point = base;
  move_ip (ip_mv);
  return return_value;
}     

FACT_t
defunc (func_t *scope, syn_tree_t expression)
{
  int b_count;
  FACT_t return_value;
  FACT_t temp;
  unsigned long i;

  return_value.f_point = alloc_func ();
  return_value.type = FUNCTION_TYPE;
  expression.syntax += get_ip ();

  if (!tokcmp (expression.syntax[0], "["))
    {
      // Check for a redeclaration
      for (b_count = i = 1; b_count > 0; i++)
        {
        another_dimension:
          if (!tokcmp (expression.syntax[i], "["))
            b_count++;
          else if (!tokcmp (expression.syntax[i], "]"))
            b_count--;
        }
      if (!tokcmp (expression.syntax[i], "["))
        goto another_dimension;
      else if (get_local_var (scope, expression.syntax[i]) != NULL)
        {
          expression.syntax += i;
          FACT_throw (scope, combine_strs (expression.syntax[0], " already declared as a local variable"), expression);
        }
      
      temp = defunc_array (NULL, scope, expression);
      if (temp.type != ERROR_TYPE)
	{
	  return_value.f_point = add_func (scope, temp.f_point->array_up->name);
	  return_value.f_point->line = scope->line;
	  return_value.f_point->array_up = temp.f_point->array_up;
	  return_value.f_point->up = scope;
	  return_value.f_point->file_name = scope->file_name; 
	  mpz_set (return_value.f_point->array_size, temp.f_point->array_size);
	  FACT_free (temp.f_point);
	}
      else
	return_value = temp;
    }
  else
    {
      if (get_local_var (scope, expression.syntax[0]) != NULL)
        FACT_throw (scope, combine_strs (expression.syntax[0], " already declared as a local variable"), expression);

      return_value.f_point = add_func (scope, expression.syntax[0]);
      if (return_value.f_point == NULL)
	FACT_throw (scope, "could not define function", expression);

      return_value.f_point->line = scope->line;
      return_value.f_point->up = scope;
      return_value.f_point->file_name = scope->file_name; 
      mpz_set_ui (return_value.f_point->array_size, 1);
    }

  next_inst ();
  return return_value;
}

FACT_t
def_array (var_t *base, func_t *scope, syn_tree_t expression)
{
  mpz_t  i;
  mpz_t  size;
  FACT_t checker;
  FACT_t array_size;
  FACT_t return_value;
  unsigned long ip_mv;
  unsigned long ip;
  var_t *curr;
  
  return_value.v_point = alloc_var ();
  return_value.type = VAR_TYPE;

  if (base == NULL)
    base = alloc_var ();

  if (tokcmp (expression.syntax[0], "["))
    {
      ip_mv = 1;
      base->name = expression.syntax[0];
    }
  else
    {
      expression.syntax++;

      ip = get_ip ();

      array_size = get_array_size (scope, expression);
      ip_mv = get_ip () - ip;
      expression.syntax += get_ip () - ip;
      
      if (array_size.type == ERROR_TYPE)
	return array_size;
      if (array_size.type != VAR_TYPE)
	FACT_throw (scope, "array size needs to be a variable", expression);

      mpz_init    (size);
      mpc_get_mpz (size, array_size.v_point->data);

      if (mpz_cmp_ui (size, 1) <= 0)
	FACT_throw (scope, "invalid array size", expression);
      
      base = resize_array (base, size);
      curr = base->array_up;
      for (mpz_init (i); mpz_cmp (i, size) < 0; mpz_add_ui (i, i, 1))
	{
	  set_ip (ip);	  
	  checker = def_array (curr, scope, expression);
	  curr = curr->next;

          if (checker.type == ERROR_TYPE)
	    return checker;
	}
      base->name = base->array_up->name;
    }
  return_value.v_point = base;
  move_ip (ip_mv);
  return return_value;
}     

FACT_t
define (func_t *scope, syn_tree_t expression)
{
  int b_count;
  FACT_t return_value;
  FACT_t temp;
  unsigned long i;

  return_value.v_point = alloc_var ();
  return_value.type = VAR_TYPE;
  expression.syntax += get_ip ();

  if (!tokcmp (expression.syntax[0], "["))
    {
      // Check for a redeclaration
      for (b_count = i = 1; b_count > 0; i++)
        {
        another_dimension:
          if (!tokcmp (expression.syntax[i], "["))
            b_count++;
          else if (!tokcmp (expression.syntax[i], "]"))
            b_count--;
        }
      if (!tokcmp (expression.syntax[i], "["))
        goto another_dimension;
      else if (get_local_func (scope, expression.syntax[i]) != NULL)
        {
          expression.syntax += i;
          FACT_throw (scope, combine_strs (expression.syntax[0], " already declared as a local function"), expression);
        }
      
      temp = def_array (NULL, scope, expression);

      if (temp.type != ERROR_TYPE)
	{
	  return_value.v_point = add_var (scope, temp.v_point->array_up->name);
	  return_value.v_point->array_up = temp.v_point->array_up;
	  mpz_set (return_value.v_point->array_size, temp.v_point->array_size);
	}
      else
	return_value = temp;
    }
  else
    {
      if (get_local_func (scope, expression.syntax[0]) != NULL)
        FACT_throw (scope, combine_strs (expression.syntax[0], " already declared as a local function"), expression);
      return_value.v_point = add_var (scope, expression.syntax[0]);
    }

  next_inst ();
  return return_value;
}

var_t *
clone_var (var_t *surrogate, char *name)
{
  var_t *clone;

  if (surrogate == NULL)
    return NULL;

  clone = alloc_var ();
  clone->name = name;
  clone->next = clone_var (surrogate->next, name);
  clone->array_up = clone_var (surrogate->array_up, name);

  mpz_set (clone->array_size, surrogate->array_size);
  mpc_set (&(clone->data), surrogate->data);

  return clone;
}

var_t *
clone_var_f (var_t *surr, char *name)
{
  // Just to simplify things
  var_t *hold_n;
  var_t *hold_r;

  if (surr == NULL)
    return NULL;

  hold_n = surr->next;
  surr->next = NULL;
  hold_r = clone_var (surr, name);
  surr->next = hold_n;

  return hold_r;
}

FACT_t
set (func_t *scope, syn_tree_t expression)
{
  FACT_t arg1;
  FACT_t arg2;
  var_t *hold;
  var_t *copy;
  
  if ((arg1 = eval (scope, expression)).type == ERROR_TYPE)
    return arg1;
  if ((arg2 = eval (scope, expression)).type == ERROR_TYPE)
    return arg2;

  if (arg1.type == VAR_TYPE)
    {
      if (arg2.type == FUNCTION_TYPE)
	FACT_throw (scope, "cannot set a var to a function", expression);
      if (arg1.v_point->locked)
	return arg2;
      
      hold = arg2.v_point->next;
      arg2.v_point->next = NULL;
      copy = clone_var (arg2.v_point, arg1.v_point->name);
      arg2.v_point->next = hold;
      
      free_var (arg1.v_point->array_up);
      arg1.v_point->array_up = copy->array_up;      
      mpz_set (arg1.v_point->array_size, copy->array_size);
      mpc_set (&(arg1.v_point->data), copy->data);
    }
  else if (arg1.type == FUNCTION_TYPE)
    {
      if (arg2.type == VAR_TYPE)
	FACT_throw (scope, "cannot set a function to a var", expression);
      if (arg1.f_point->locked)
	return arg2;

      arg1.f_point->line = arg2.f_point->line;
      arg1.f_point->file_name = arg2.f_point->file_name;
      arg1.f_point->args = arg2.f_point->args;
      arg1.f_point->body = arg2.f_point->body;
      arg1.f_point->usr_data = arg2.f_point->usr_data;
      arg1.f_point->extrn_func = arg2.f_point->extrn_func;
      arg1.f_point->vars = arg2.f_point->vars;
      arg1.f_point->funcs = arg2.f_point->funcs;
      arg1.f_point->up = arg2.f_point->up;
      arg1.f_point->caller = arg2.f_point->caller;
      arg1.f_point->array_up = arg2.f_point->array_up;
      arg1.f_point->variadic = arg2.f_point->variadic;
      
      mpz_set (arg1.f_point->array_size, arg2.f_point->array_size);
    }

  return arg1;
}

FACT_t
get_array_size (func_t *scope, syn_tree_t expression)
{
  /* get_array_size - get whatever is returned in between
   * [ and ]. Simple, easy, not all that big of a deal.
   */
  int           len;
  FACT_t        return_value;
  unsigned long ip;

  /* We hold on to the ip but reset it. This is to make eval
   * think that expression is at zero.
   */
  ip = get_ip ();
  reset_ip ();

  len = get_exp_length (expression.syntax, ']');
  return_value = eval (scope, expression);
  set_ip (ip + len);

  return return_value;
}

FACT_t
return_array (func_t *scope, syn_tree_t expression)
{
  /* return_array - create an anonymous variable or function array, depending
   * on the first returned variable. Primitive format for return_array is
   * '![' arg0 ',' arg1 ',' ... ',' argN ']'. The token '[' will automatically
   * be converted to a '![' if it does not follow a number/string/variable or
   * function.
   */
  mpz_t  size;
  FACT_t return_value;
  FACT_t hold;
  type_t type;
  unsigned long ip;

  void *curr;
  void *base;
  
  ip = get_ip ();
  expression.syntax += ip;
  reset_ip ();

  /* Get the initial argument, the one that determines the type of
   * the array.
   */
  hold = eval (scope, expression);
  if (hold.type == ERROR_TYPE)
    return hold;

  type = hold.type;
  // To avoid a nasty ? conditional expression.
  if (type == VAR_TYPE)
    curr = (void *) clone_var_f (hold.v_point, NULL);
  else
    curr = (void *) hold.f_point;
  base = curr;
  mpz_init_set_ui (size, 1);

  for (;;)
    {
      ip += get_ip ();
      expression.syntax += get_ip ();
      reset_ip ();
      
      if (!tokcmp (expression.syntax[0], "]"))
	{
	  ip++;
	  break;
	}

      next_inst ();
      hold = eval (scope, expression);

      if (hold.type == ERROR_TYPE)
        return hold;
      else if (type != hold.type)
	{
          FACT_throw (scope, ((type == VAR_TYPE)
                              ? "unexpected function returned in a variable array"
                              : "unexpected variable returned in a function array"), expression);
	}

      if (type == VAR_TYPE)
	{
	  ((var_t *) curr)->next = clone_var_f (hold.v_point, NULL);
          curr = (void *) (((var_t *) curr)->next);
	}
      else 
	{
	  ((func_t *) curr)->next = hold.f_point;
	  curr = (void *) (((func_t *) curr)->next);
	}
      mpz_add_ui (size, size, 1);
    }

  return_value.type = type;

  if (type == VAR_TYPE)
    {
      if (!mpz_cmp_ui (size, 1))
	return_value.v_point = (var_t *) base;
      else
        {
          return_value.v_point = alloc_var ();
          return_value.v_point->array_up = (var_t *) base;
          mpz_set (return_value.v_point->array_size, size);
        }
    }
  else 
    {
      if (!mpz_cmp_ui (size, 1))
	return_value.f_point = (func_t *) base;
      else
        {
          return_value.f_point = alloc_func ();
          return_value.f_point->array_up = (func_t *) base;
          mpz_set (return_value.f_point->array_size, size);
        }
    }

  // Set the instruction pointer back.
  set_ip (ip);
  return return_value;
}

FACT_t
get_array_var (var_t *root, func_t *scope, syn_tree_t expression)
{
  /* get_array_var - get the element of a variable array, if
   * requested. This function is always called from eval after
   * a variable is returned.
   */
  mpz_t  i;
  mpz_t  size;
  FACT_t return_value;
  FACT_t array_size;
  unsigned long ip;

  return_value.type = VAR_TYPE;
  ip = get_ip ();
  expression.syntax += ip;

  /* If the expression ends with the variable or there is no
   * trailing [, return the variable alone.
   */
  if (expression.syntax[0] == NULL || tokcmp (expression.syntax[0], "["))
    {
      return_value.v_point = root;
      return return_value;
    }
  
  /* Move the instruction pointer and the expression over one. */
  set_ip (++ip); // We don't use next_inst here because this is easier.
  expression.syntax++;
  array_size = get_array_size (scope, expression);

  /* Move the instructions forward and set the ip to zero. */
  expression.syntax += get_ip () - ip;
  ip = get_ip ();
  reset_ip ();
    
  if (array_size.type == ERROR_TYPE)
    return array_size;
  
  if (array_size.type != VAR_TYPE)
    FACT_throw (scope, "array position cannot be a function", expression);

  mpz_init (size);
  mpc_get_mpz (size, array_size.v_point->data);

  if (mpz_cmp (size, root->array_size) >= 0 || mpz_cmp_ui (size, 0) < 0)
    FACT_throw (scope, "array out of bounds", expression);

  if (!mpz_cmp_ui (root->array_size, 1))
    {
      if (expression.syntax[0] != NULL && !tokcmp (expression.syntax[0], "["))
	FACT_throw (scope, "array out of bounds", expression);

      return_value.v_point = root;
      move_ip (ip);
      return return_value;
    }

  for (root = root->array_up,
	 mpz_init (i); mpz_cmp (i, size) < 0; mpz_add_ui (i, i, 1))
    root = root->next;

  return_value = get_array_var (root, scope, expression);
  move_ip (ip);
  
  return return_value;
}

FACT_t
get_array_func (func_t *root, func_t *scope, syn_tree_t expression)
{
  /* get_array_func - get the element of a function array. Man, a
   * lot of these functions should probably be combined or 
   * something. No comments in this one again, look at
   * get_array_var. It operates in almost the exact same way.
   */
  mpz_t  i;
  mpz_t  size;
  FACT_t return_value;
  FACT_t array_size;
  unsigned long ip;

  return_value.type = FUNCTION_TYPE;

  ip = get_ip ();
  expression.syntax += ip;

  if (expression.syntax[0] == NULL || tokcmp (expression.syntax[0], "["))
    {
      return_value.f_point = root;
      return return_value;
    }

  set_ip (++ip);
  expression.syntax++;

  array_size = get_array_size (scope, expression);

  expression.syntax += get_ip () - ip;
  ip = get_ip ();
  reset_ip ();

  if (array_size.type == ERROR_TYPE)
    return array_size;
  
  if (array_size.type != VAR_TYPE)
    FACT_throw (scope, "array position cannot be a function", expression);

  mpz_init    (size);
  mpc_get_mpz (size, array_size.v_point->data);

  if (mpz_cmp (size, root->array_size) >= 0 || mpz_cmp_ui (size, 0) < 0)
    FACT_throw (scope, "array out of bounds", expression);

  if (!mpz_cmp_ui (root->array_size, 1))
    {
      if (expression.syntax[1] != NULL && !tokcmp (expression.syntax[1], "["))
	FACT_throw (scope, "array out of bounds", expression);

      return_value.f_point = root;
      move_ip (ip);
      return return_value;
    }

  for (root = root->array_up,
	 mpz_init (i); mpz_cmp (i, size) < 0; mpz_add_ui (i, i, 1))
    root = root->next;

  return_value = get_array_func (root, scope, expression);
  move_ip (ip);

  return return_value;
}

FACT_t
combine_arrays (FACT_t op1, FACT_t op2)
{
  mpz_t  i;
  FACT_t return_value;
  var_t *result;
  var_t *temp1;
  var_t *temp2;
  var_t *hold;

  hold = op1.v_point->next;
  op1.v_point->next = NULL;
  temp1 = clone_var (op1.v_point, "temp1");
  op1.v_point->next = hold;
  hold = op2.v_point->next;
  op2.v_point->next = NULL;
  temp2 = clone_var (op2.v_point, "temp2");
  op2.v_point->next = hold;
  result = alloc_var ();

  mpz_add (result->array_size, temp1->array_size, temp2->array_size);

  if (!mpz_cmp_ui (temp1->array_size, 1))
    result->array_up = temp1;
  else
    result->array_up = temp1->array_up;

  for (hold = result->array_up, mpz_init_set_ui (i, 1); mpz_cmp (temp1->array_size, i); hold = hold->next, mpz_add_ui (i, i, 1));

  if (!mpz_cmp_ui (temp2->array_size, 1))
    hold->next = temp2;
  else
    hold->next = temp2->array_up;

  return_value.type = VAR_TYPE;
  return_value.v_point = result;

  return return_value;
}
