#include "FACT.h"

FACT_t
defunc_array (func_t *base, func_t *scope, word_list expression)
{
  /* defunc_array - create a function array. This function is
   * called be defunc in order to more easily create mult-
   * dimensionaly arrays. Currently there is only one bug,
   * and that is dimensions of size one will cause a segfault.
   * It's not a very good solution to increase the minimum
   * dimension size (which is 0), this surprisingly has a bit
   * of use, which I will not go into now.
   * Also, this function is recursive. While that may seem like
   * an issue, it only calls itself recursively once per
   * dimension. An array would have to have something like
   * over 10000 dimensions to cause this.
   */
  mpz_t           size         ;
  mpz_t           index        ;
  FACT_t          checker      ;
  FACT_t          array_size   ;
  FACT_t          return_value ;
  func_t        * scroller     ;
  unsigned long   ip_mv        ;
  unsigned long   ip           ;

  scope->line += expression.lines[0];

  /* Set the return value defaults. */
  return_value.f_point = alloc_func ();
  return_value.type    = FUNCTION_TYPE;

  if (expression.syntax[0] == NULL)
    return errorman_throw_reg (scope, "cannot make an anonymous function array");

  /* If this is the first dimension, allocate the base. */
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
      expression.lines++;
      
      scope->line += expression.lines[0];

      ip = get_ip ();

      /* Get the array size, self explanatory. */
      array_size = get_array_size (scope, expression);

      /* Move the expression forward only by the difference in positions
       * between the last token evaluated by get_array_size and the
       * current token.
       */
      ip_mv              = get_ip () - ip;
      expression.syntax += get_ip () - ip;
      expression.lines  += get_ip () - ip;

      /* If there was an error in the dimension declaration, we return
       * it. If get_array_size returned a function (get_array_size
       * works essentially the same way as parenthesis) then we
       * return an error.
       */
      if (array_size.type == ERROR_TYPE)
	return array_size;
      if (array_size.type != VAR_TYPE)
	return errorman_throw_reg (scope, "array size needs to be a variable");

      /* Set array_size to an mpc_t and truncate it. */
      mpz_init    (size);
      mpc_get_mpz (size, array_size.v_point->data);

      /* If the array size is out of bounds, return an error. */
      if (mpz_cmp_ui (size, 1) < 0)
	return errorman_throw_reg (scope, "invalid array size");

      base     = resize_func_array (base, size);
      scroller = base->array_up;
      for (mpz_init (index);  mpz_cmp (index, size) < 0; mpz_add_ui (index, index, 1))
	{
	  /* Reset the ip if it has moved at all. This is probably the
	   * case if there were dimensions beyond this one. Do not do
	   * This for the last loop, as we want it to carry over.
	   */
	  set_ip (ip);
	  
	  checker      = defunc_array (scroller, scope, expression);
	  scope->line -= expression.lines[0];
	  scroller     = scroller->next;

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
defunc (func_t *scope, word_list expression)
{
  /* defunc - define a function. This one's really simple,
   * most of the difficult stuff comes from either something
   * in management.c or defunc_array.
   */
  FACT_t return_value;
  FACT_t temp;

  return_value.f_point = alloc_func ();
  return_value.type    = FUNCTION_TYPE;

  expression.syntax += get_ip ();
  expression.lines  += get_ip ();

  /* This will be removed in the near future. */
  if (expression.syntax[0] == NULL)
    return errorman_throw_reg (scope, "cannot define anonymous functions");

  return_value.type = FUNCTION_TYPE;

  if (!tokcmp (expression.syntax[0], "["))
    {
      temp = defunc_array (NULL, scope, expression);

      if (temp.type != ERROR_TYPE)
	{
	  return_value.f_point            = add_func (scope, temp.f_point->array_up->name);
	  return_value.f_point->line      = scope->line;
	  return_value.f_point->array_up  = temp.f_point->array_up;
	  return_value.f_point->up        = scope;
	  return_value.f_point->file_name = scope->file_name; 
	  
	  mpz_set (return_value.f_point->array_size, temp.f_point->array_size);
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

      return_value.f_point->line      = scope->line;
      return_value.f_point->up        = scope;
      return_value.f_point->file_name = scope->file_name; 
      mpz_set_ui (return_value.f_point->array_size, 1);
    }

  next_inst ();
  return return_value;
}

FACT_t
def_array (var_t *base, func_t *scope, word_list expression)
{
  /* def_array - creates a variable array. This function works in
   * almost the EXACT SAME WAY as defunc_array, which is why I'm
   * not going to comment it as much, if at all.
   */
  mpz_t    size;
  mpz_t    index;
  var_t  * scroller;
  FACT_t   checker;
  FACT_t   array_size;
  FACT_t   return_value;
  unsigned long ip_mv;
  unsigned long ip;
  
  scope->line += expression.lines[0];

  return_value.v_point = alloc_var ();
  return_value.type    = VAR_TYPE;

  if (expression.syntax[0] == NULL)
    return errorman_throw_reg (scope, "cannot define anonymous array");

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
      expression.lines++;
      
      scope->line += expression.lines[0];

      ip = get_ip ();

      array_size = get_array_size (scope, expression);

      ip_mv              = get_ip () - ip;
      expression.syntax += get_ip () - ip;
      expression.lines  += get_ip () - ip;
      
      if (array_size.type == ERROR_TYPE)
	return array_size;
      
      if (array_size.type != VAR_TYPE)
	return errorman_throw_reg (scope, "array size needs to be a variable");

      mpz_init    (size);
      mpc_get_mpz (size, array_size.v_point->data);

      if (mpz_cmp_ui (size, 1) < 0)
	return errorman_throw_reg (scope, "invalid array size");
      
      if (mpz_cmp_ui (size, 1) == 0)
	{
	  /* If the array size is 1, there's nothing to extend or
	   * assign, thus we continue to look for the name of the
	   * array and additional dimension, ignoring this one.
	   *
	   * ... This doesn't work because I am an idiot.
	   */
	  return def_array (NULL, scope, expression);
	}

      base     = resize_array (base, size);
      scroller = base->array_up;
      for (mpz_init (index); mpz_cmp (index, size) < 0; mpz_add_ui (index, index, 1))
	{
	  set_ip (ip);
	  
	  checker      = def_array (scroller, scope, expression);
	  scope->line -= expression.lines[0];
	  scroller     = scroller->next;
	  
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
define (func_t *scope, word_list expression)
{
  /* define - define a variable. Again, this works in almost
   * the exact same way as defunc.
   */
  FACT_t return_value;
  FACT_t temp;

  return_value.v_point = alloc_var ();
  return_value.type    = VAR_TYPE;

  expression.syntax += get_ip ();
  expression.lines  += get_ip ();

  if (!tokcmp (expression.syntax[0], "["))
    {
      temp = def_array (NULL, scope, expression);

      if (temp.type != ERROR_TYPE)
	{
	  return_value.v_point             = add_var (scope, temp.v_point->array_up->name);
	  return_value.v_point->array_up   = temp.v_point->array_up;
	  
	  mpz_set (return_value.v_point->array_size, temp.v_point->array_size);
	}
      else
	return_value = temp;
    }
  else
    return_value.v_point = add_var (scope, expression.syntax[0]);

  next_inst ();
  return return_value;
}

var_t *
clone_var (var_t *surrogate, char *name)
{
  var_t * clone;

  if (surrogate == NULL)
    return NULL;

  clone             = alloc_var ();
  clone->name       = name;
  
  clone->next       = clone_var (surrogate->next, name);
  clone->array_up   = clone_var (surrogate->array_up, name);

  mpz_set (clone->array_size, surrogate->array_size);
  mpc_set (&(clone->data), surrogate->data);

  return clone;
}

FACT_t
set (func_t *scope, word_list expression)
{
  var_t  * hold;
  var_t  * copy;
  FACT_t   arg1;
  FACT_t   arg2;
  
  if ((arg1 = eval (scope, expression)).type == ERROR_TYPE)
    return arg1;
  if ((arg2 = eval (scope, expression)).type == ERROR_TYPE)
    return arg2;

  if (arg1.type == VAR_TYPE)
    {
      if (arg2.type == FUNCTION_TYPE)
	return errorman_throw_reg (scope, "cannot set a var to a function");
      if (arg1.v_point->locked)
	return arg2;
      
      hold               = arg2.v_point->next;
      arg2.v_point->next = NULL;
      copy               = clone_var (arg2.v_point, arg1.v_point->name);
      arg2.v_point->next = hold;
      
      free_var (arg1.v_point->array_up);

      arg1.v_point->array_up = copy->array_up;
      
      mpz_set (arg1.v_point->array_size, copy->array_size);
      mpc_set (&(arg1.v_point->data), copy->data);
    }
  else if (arg1.type == FUNCTION_TYPE)
    {
      if (arg2.type == VAR_TYPE)
	return errorman_throw_reg (scope, "cannot set a function to a var");
      if (arg1.f_point->locked)
	return arg2;

      arg1.f_point->line       = arg2.f_point->line       ;
      arg1.f_point->file_name  = arg2.f_point->file_name  ;
      arg1.f_point->args       = arg2.f_point->args       ;
      arg1.f_point->body       = arg2.f_point->body       ;
      arg1.f_point->usr_data   = arg2.f_point->usr_data   ;
      arg1.f_point->extrn_func = arg2.f_point->extrn_func ;
      arg1.f_point->vars       = arg2.f_point->vars       ;
      arg1.f_point->funcs      = arg2.f_point->funcs      ;
      arg1.f_point->up         = arg2.f_point->up         ;
      arg1.f_point->caller     = arg2.f_point->caller     ;
      arg1.f_point->array_up   = arg2.f_point->array_up   ;
      arg1.f_point->variadic   = arg2.f_point->variadic   ;

      mpz_set (arg1.f_point->array_size, arg2.f_point->array_size);
    }

  return arg1;
}

FACT_t
get_array_size (func_t *scope, word_list expression)
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

  /*
  if (!len == 0)
    return errorman_throw_reg (scope, "cannot evaluate empty brackets");

  expression.syntax[index - 1] = NULL;
  */
  return_value = eval (scope, expression);
  //expression.syntax[index - 1] = "]";

  /* Set the ip back to whatever it was. */
  set_ip (ip + len);

  return return_value;
}

FACT_t
return_array (func_t *scope, word_list expression)
{
  /* return_array - create an anonymous variable or function array, depending
   * on the first returned variable. Primitive format for return_array is
   * '![' arg0 ',' arg1 ',' ... ',' argN ']'. The token '[' will automatically
   * be converted to a '![' if it does not follow a number/string/variable or
   * function.
   */
  mpz_t         size         ;
  FACT_t        return_value ;
  FACT_t        hold         ;
  type_define   type         ;
  unsigned long ip           ;
  
  /* Something tells me that it was sort of
   * pointless to use unions in this case, and
   * I'll probably remove them at a later date.
   * But, alas, this project was designed so
   * that I could learn C from it, and this
   * helped, so it was included.
   */
  union
  {
    var_t  * var_t_root;
    func_t * func_t_root;
  } roots;
  union
  {
    var_t  * var_t_value;
    func_t * func_t_value;
  } values;

  ip = get_ip ();
  expression.syntax += ip;
  expression.lines  += ip;
 
  if (!tokcmp (expression.syntax[0], "]"))
    {
      /* Perhaps here, a better action would be to create the empty
       * set? Unfortunately, I don't think that'd be a good idea.
       */
      return errorman_throw_reg (scope, "expected body before ']'");
    }

  reset_ip ();

  /* Get the initial argument, the one that determines the type of
   * the array.
   */
  if ((hold = eval (scope, expression)).type == ERROR_TYPE)
    return hold;

  type = hold.type;

  if (type == VAR_TYPE)
    {
      values.var_t_value         = hold.v_point       ;
      roots.var_t_root           = alloc_var ()       ;
      roots.var_t_root->array_up = values.var_t_value ;
    }
  else if (type == FUNCTION_TYPE)    
    {
      values.func_t_value         = hold.f_point        ;
      roots.func_t_root           = alloc_func ()       ;
      roots.func_t_root->up       = scope               ;
      roots.func_t_root->array_up = values.func_t_value ;
    }

  mpz_init_set_ui (size, 1);

  for (;;)
    {
      ip += get_ip ();
      expression.syntax += get_ip ();
      expression.lines  += get_ip ();
      reset_ip ();
      
      if (!tokcmp (expression.syntax[0], "]"))
	{
	  scope->line += expression.lines[0];
	  next_inst ();
	  break;
	}
      else if (tokcmp (expression.syntax[0], ","))
	return errorman_throw_reg (scope, "expected ',' or closing ']'");

      /* If there was a comma, then move the instruction pointer forward one
       * and get the next argument.
       */
      next_inst ();      

      if ((hold = eval (scope, expression)).type == ERROR_TYPE)
	return hold;
      else if (type != hold.type)
	{
	  if (type == VAR_TYPE)
	    return errorman_throw_reg (scope, "unexpected function returned in a variable array");
	  else
	    return errorman_throw_reg (scope, "unexpected variable returned in a function array");
	}

      if (type == VAR_TYPE)
	{
	  values.var_t_value->next = hold.v_point;
	  values.var_t_value       = values.var_t_value->next;
	}
      else if (type == FUNCTION_TYPE)
	{
	  values.func_t_value->next = hold.f_point;
	  values.func_t_value       = values.func_t_value->next;
	}
      mpz_add_ui (size, size, 1);
    }

  return_value.type = type;

  if (type == VAR_TYPE)
    {
      if (!mpz_cmp_ui (size, 1))
	return_value.v_point = roots.var_t_root->array_up;
      else
	return_value.v_point = roots.var_t_root;

      mpz_set (return_value.v_point->array_size, size);
    }
  else if (type == FUNCTION_TYPE)
    {
      if (!mpz_cmp_ui (size, 1))
	return_value.f_point = roots.func_t_root->array_up;
      else
	return_value.f_point = roots.func_t_root;

      mpz_set (return_value.f_point->array_size, size);
    }

  /* Set the instruction pointer back. */
  set_ip (ip);

  return return_value;
}

FACT_t
size_of (func_t *scope, word_list expression)
{
  FACT_t return_value;
  FACT_t evald;
  
  if (expression.syntax[0] == NULL)
    return errorman_throw_reg (scope, "cannot find the sizeof nothing");

  evald = eval (scope, expression);

  if (evald.type == ERROR_TYPE)
    return evald;

  return_value.v_point = alloc_var ();
  return_value.type    = VAR_TYPE;
  

  if (evald.type == VAR_TYPE)
    mpz_set (return_value.v_point->data.object, evald.v_point->array_size);
  else
    mpz_set (return_value.v_point->data.object, evald.f_point->array_size);

  return_value.v_point->data.precision = 0;
  
  return return_value;
}

FACT_t
get_array_var (var_t *root, func_t *scope, word_list expression)
{
  /* get_array_var - get the element of a variable array, if
   * requested. This function is always called from eval after
   * a variable is returned.
   */
  mpz_t  size;
  mpz_t  index;
  FACT_t return_value;
  FACT_t array_size;
  unsigned long ip;

  return_value.type = VAR_TYPE;

  /* Get the instruction pointer and move forward. */
  ip = get_ip ();
  expression.syntax += ip;
  expression.lines  += ip;

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
  expression.lines++;

  array_size = get_array_size (scope, expression);

  /* Move the instructions forward and set the ip to zero. */
  expression.syntax += get_ip () - ip;
  expression.lines  += get_ip () - ip;
  ip = get_ip ();
  reset_ip ();
    
  if (array_size.type == ERROR_TYPE)
    return array_size;
  
  if (array_size.type != VAR_TYPE)
    return errorman_throw_reg (scope, "array position cannot be a function");

  mpz_init    (size);
  mpc_get_mpz (size, array_size.v_point->data);

  if (mpz_cmp (size, root->array_size) >= 0 || mpz_cmp_ui (size, 0) < 0)
    return errorman_throw_reg (scope, "array out of bounds");

  if (!mpz_cmp_ui (root->array_size, 1))
    {
      if (expression.syntax[0] != NULL && !tokcmp (expression.syntax[0], "["))
	return errorman_throw_reg (scope, "array out of bounds");

      return_value.v_point = root;

      /* Move the ip forward again. */
      move_ip (ip);
      return return_value;
    }

  for (root = root->array_up,
	 mpz_init (index); mpz_cmp (index, size) < 0; mpz_add_ui (index, index, 1))
    root = root->next;

  /* Check for any more dimensions being accessed. */
  return_value = get_array_var (root, scope, expression);
  /* Move the ip forward again. */
  move_ip (ip);
  
  return return_value;
}

FACT_t
get_array_func (func_t *root, func_t *scope, word_list expression)
{
  /* get_array_func - get the element of a function array. Man, a
   * lot of these functions should probably be combined or 
   * something. No comments in this one again, look at
   * get_array_var. It operates in almost the exact same way.
   */
  mpz_t  size;
  mpz_t  index;
  FACT_t return_value;
  FACT_t array_size;
  unsigned long ip;

  return_value.type = FUNCTION_TYPE;

  ip = get_ip ();
  expression.syntax += ip;
  expression.lines  += ip;

  if (expression.syntax[0] == NULL || tokcmp (expression.syntax[0], "["))
    {
      return_value.f_point = root;
      return return_value;
    }

  set_ip (++ip);
  expression.syntax++;
  expression.lines++;

  array_size = get_array_size (scope, expression);

  expression.syntax += get_ip () - ip;
  expression.lines  += get_ip () - ip;
  ip = get_ip ();
  reset_ip ();

  if (array_size.type == ERROR_TYPE)
    return array_size;
  
  if (array_size.type != VAR_TYPE)
    return errorman_throw_reg (scope, "array position cannot be a function");

  mpz_init    (size);
  mpc_get_mpz (size, array_size.v_point->data);

  if (mpz_cmp (size, root->array_size) >= 0 || mpz_cmp_ui (size, 0) < 0)
    return errorman_throw_reg (scope, "array out of bounds");

  if (!mpz_cmp_ui (root->array_size, 1))
    {
      if (expression.syntax[1] != NULL && !tokcmp (expression.syntax[1], "["))
	return errorman_throw_reg (scope, "array out of bounds");

      return_value.f_point = root;

        /* Move the ip forward again. */
      move_ip (ip);
      return return_value;
    }

  for (root = root->array_up,
	 mpz_init (index); mpz_cmp (index, size) < 0; mpz_add_ui (index, index, 1))
    root = root->next;

  return_value = get_array_func (root, scope, expression);
  move_ip (ip);

  return return_value;
}

FACT_t
combine_arrays (FACT_t op1, FACT_t op2)
{
  var_t  *result;
  var_t  *temp1;
  var_t  *temp2;
  var_t  *hold;
  mpz_t   index;
  FACT_t  return_value;

  if (op1.type == ERROR_TYPE)
    return op1;
  else if (op2.type == ERROR_TYPE)
    return op2;

  if (op1.type == FUNCTION_TYPE
      || op2.type == FUNCTION_TYPE)
    return errorman_throw_reg (NULL, "both arguments to ~ must be variables");

  hold              = op1.v_point->next;
  op1.v_point->next = NULL;
  temp1             = clone_var (op1.v_point, "temp1");
  op1.v_point->next = hold;
  hold              = op2.v_point->next;
  op2.v_point->next = NULL;
  temp2             = clone_var (op2.v_point, "temp2");
  op2.v_point->next = hold;
  result            = alloc_var ();

  mpz_add (result->array_size, temp1->array_size, temp2->array_size);

  if (!mpz_cmp_ui (temp1->array_size, 1))
    result->array_up = temp1;
  else
    result->array_up = temp1->array_up;

  for (hold = result->array_up, mpz_init_set_ui (index, 1); mpz_cmp (temp1->array_size, index); hold = hold->next, mpz_add_ui (index, index, 1));

  if (!mpz_cmp_ui (temp2->array_size, 1))
    hold->next = temp2;
  else
    hold->next = temp2->array_up;

  return_value.type    = VAR_TYPE;
  return_value.v_point = result;

  return return_value;
}
