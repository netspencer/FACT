#include "common.h"

struct _MATH_PRIMS
{ 
  const char *name;
  a_type (*function)(a_type, a_type);
};

static struct _MATH_PRIMS math_calls[] =
  {
    {"+",   add},
    {"-",   sub},
    {"*",   mult},
    {"/",   divide},
    {"%",   mod},
    {"+=",   add_assignment},
    {"-=",   sub_assignment},
    {"*=",   mult_assignment},
    {"/=",   div_assignment},
    {"%=",   mod_assignment},
    {/*"eq"*/ "==",  equal},
    {/*"nq"*/ "!=",  not_equal},
    {">",   more},
    {"<",   less},
    {/*"meq"*/ ">=", more_equal},
    {/*"leq"*/ "<=", less_equal},
    //    {/*"and"*/ "&&", and},
    //   {/*"or"*/  "||",  or}
  };

#define NUM_MATH_PRIMS ((sizeof math_calls) / (sizeof (struct _MATH_PRIMS)))

struct _prims
{ 
  const char *name;
  a_type (*function)(func *, word_list);
};

static int num_of_prims = 0; 

static struct _prims *primitives = NULL;

void
add_prim (const char *prim_name,
	  a_type (*new_function)(func *, word_list))
{
  num_of_prims++;

  primitives = (struct _prims *) better_realloc (primitives,
						 sizeof (struct _prims) * num_of_prims);

  primitives[num_of_prims - 1].name = prim_name;
  primitives[num_of_prims - 1].function = new_function;
}

void
init_std_prims (void)
{
  /* All primitives that are symbols */
  add_prim ("=", set);
  add_prim ("(", paren);
  add_prim ("{", lambda_proc);
  add_prim ("![", return_array);
  add_prim ("@", liven_func);
  add_prim ("$", run_func);
  add_prim ("&", new_scope);
  add_prim (":", in_scope);
  add_prim ("\"", new_string);
  add_prim ("?", errorman_throw_prim);
  add_prim ("&&", and);
  add_prim ("||", or);
  /* Start with 'd' */
  add_prim ("def", define);
  add_prim ("defunc", defunc);
  /* start with 'e' */
  add_prim ("else", invalid_else);
  /* start with 'f' */
  add_prim ("for", invalid_for);
  /* start with 'g' */
  add_prim ("getc", input_character);
  /* start with 'i' */
  add_prim ("if", invalid_if);
  /* start with 'l' */
  add_prim ("lambda", lambda);
  /* start with 'p' */
  add_prim ("printc", print_character);
  add_prim ("printv", print_var);
  /* start with 's' */
  add_prim ("sizeof", size_of);
  /* start with 'w' */
  add_prim ("while", invalid_while);
}

enum {
  SYMB = 0,
  STARTD = 12,
  STARTE = 14,
  STARTF = 15,
  STARTG = 16,
  STARTI = 17,
  STARTL = 18,
  STARTP = 19,
  STARTS = 21,
  STARTW = 22,
  STARTother = 23
};

int
isprim (char *word)
{
  int pos;

  if (ispunct ((int) word[0]))
    pos = SYMB;
  else switch (word[0])
	 {
	 case 'd':
	   pos = STARTD;
	   break;
	   
	 case 'e':
	   pos = STARTE;
	   break;

	 case 'f':
	   pos = STARTF;
	   break;

	 case 'g':
	   pos = STARTG;
	   break;

	 case 'i':
	   pos = STARTI;
	   break;

	 case 'l':
	   pos = STARTL;
	   break;
	   
	 case 'p':
	   pos = STARTP;
	   break;

	 case 's':
	   pos = STARTS;
	   break;

	 case 'w':
	   pos = STARTW;
	   break;

	 default:
	   pos = STARTother;
	   break;
	 }

  while (pos < num_of_prims)
    {
      if (pos >= STARTD && primitives[pos].name[0] != word[0])
	return -1;
      
      if (strcmp (primitives[pos].name, word) == 0)
	return pos;

      pos++;
    }

  return -1;
}

int
ismathcall (char *word)
{
  int pos;

  if (!ispunct ((int) word[0]))
    return -1;

  for (pos = 0; pos < NUM_MATH_PRIMS; pos++)
    {
      if (strcmp (math_calls[pos].name, word) == 0)
        return pos;
    }

  return -1;
}

a_type
runprim (func *scope, word_list expression, int prim_num)
{
  a_type return_value;

  return_value = primitives[prim_num].function (scope, expression);

  if (prim_num != 2)
    {
      return_value.isret = false;
      return_value.break_signal = false;
    }
  
  return return_value;
}

a_type
eval_math (func *scope, word_list expression, int call_num)
{
  a_type arg1;
  a_type arg2;
  a_type return_value;

  arg1 = eval (scope, expression);
  arg2 = eval (scope, expression);

  /* There has to be a better way to do this and I will implement it eventually. */
  if (call_num == 1 && arg1.type == VAR_TYPE
      && arg2.type == ERROR_TYPE && (!strcmp (arg2.error.description, "cannot evaluate ;")
				  || !strcmp (arg2.error.description, "cannot evaluate <-")
				     || !strcmp (arg2.error.description, "cannot evaluate empty expression")))
    {
      mpc_neg (&(arg1.v_point->data), arg1.v_point->data);
      return arg1;
    }
  
  if (arg1.type == ERROR_TYPE)
    return arg1;
  if (arg2.type == ERROR_TYPE)
    return arg2;

  return_value = math_calls[call_num].function (arg1, arg2);

  if (return_value.type == ERROR_TYPE)
    return_value.error.scope = scope;

  /* expression.move_forward[0] = 2; */
  
  return_value.isret = false;
  return_value.break_signal = false;

  return return_value;
}
