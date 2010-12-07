#include "common.h"

/*
  I think I should remove the way primitives (not math calls)
  are handled. It is rather silly.
*/

struct          _MATH_PRIMS
{ 
  const char * name;
  FACT_t    (* function)(FACT_t, FACT_t);
};

static struct _MATH_PRIMS math_calls[] =
  {
    {"~" , combine_arrays },   
    {"+" , add            },
    {"-" , sub            },
    {"*" , mult           },
    {"/" , divide         },
    {"%" , mod            },
    {"`" , bit_and        },
    {"^" , bit_xor        },
    {"|" , bit_ior        },
    {"+=", add_assignment },
    {"-=", sub_assignment },
    {"*=", mult_assignment},
    {"/=", div_assignment },
    {"%=", mod_assignment },
    {"==", equal          },
    {"!=", not_equal      },
    {">" , more           },
    {"<" , less           },
    {">=", more_equal     },
    {"<=", less_equal     },
  };

#define NUM_MATH_PRIMS ((sizeof math_calls) / (sizeof (struct _MATH_PRIMS)))

struct         _prims
{
  int          prim_num;
  const char * name;
  FACT_t    (* function)(func_t *, word_list);
};
static int             num_of_prims = 0;
static struct _prims * primitives = NULL;

static int
comp_prims (const void *op1, const void *op2)
{
  struct _prims * p1;
  struct _prims * p2;

  p1 = (struct _prims *) op1;
  p2 = (struct _prims *) op2;

  return tokcmp (p1->name, p2->name);
}

void
add_prim (const char * prim_name,
	  FACT_t    (* new_function)(func_t *, word_list))
{
  num_of_prims++;

  primitives = (struct _prims *) better_realloc (primitives,
						 sizeof (struct _prims) * num_of_prims);
  primitives[num_of_prims - 1].name     = prim_name;
  primitives[num_of_prims - 1].function = new_function;
}

void
init_std_prims (func_t *scope)
{
  func_t *import;
  
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
  add_prim ("&&", and);
  add_prim ("||", or);
  add_prim ("def", define);
  add_prim ("defunc", defunc);
  add_prim ("else", invalid_else);
  add_prim ("for", invalid_for);
  add_prim ("if", invalid_if);
  add_prim ("on_error", invalid_on_error);
  add_prim ("sizeof", size_of);
  add_prim ("while", invalid_while);
  add_prim ("lambda", lambda);
  add_prim ("up", up);
  add_prim ("this", this);
  add_prim ("NULL", NULL_func);
  add_prim ("lib.std.getchar", input_character); 
  add_prim ("lib.std.putchar", print_character);
  add_prim ("lib.std.putvar", print_var);
  import = add_func (scope, "import");
  import->args = get_words ("def path "); /* the extra space is required for parsing reasons. */
  import->extrn_func = (void * (*) (struct _FUNC *)) load_lib;
}

int
isprim (char *word)
{
  int             pos;
  static int      last_prim_count = 0;
  struct _prims   key;
  struct _prims * result;

  if (last_prim_count != num_of_prims)
    {
      qsort (primitives, num_of_prims, sizeof (struct _prims), comp_prims);

      for (pos = 0; pos < num_of_prims; pos++)
	primitives[pos].prim_num = pos;

      last_prim_count = num_of_prims;
    }

  key.name = word; 
  result = bsearch (&key, primitives, num_of_prims, sizeof (struct _prims), comp_prims);

  if (result == NULL)
    return -1;
  else
    return result->prim_num;
}

int
ismathcall (char *word)
{
  int pos;

  if (!ispunct ((int) word[0]))
    return -1;

  for (pos = 0; pos < NUM_MATH_PRIMS; pos++)
    {
      if (tokcmp (math_calls[pos].name, word) == 0)
        return pos;
    }

  return -1;
}

FACT_t
runprim (func_t *scope, word_list expression, int prim_num)
{
  FACT_t return_value;

  return_value = primitives[prim_num].function (scope, expression);

  if (prim_num != 2)
    {
      return_value.return_signal = false;
      return_value.break_signal  = false;
    }
  
  return return_value;
}

FACT_t
eval_math (func_t *scope, word_list expression, int call_num)
{
  FACT_t arg1;
  FACT_t arg2;
  FACT_t return_value;

  arg1 = eval (scope, expression);
  arg2 = eval (scope, expression);

  /* There is a better way to do this and I will implement it eventually. */
  if (call_num == 1 && arg1.type == VAR_TYPE
      && arg2.type == ERROR_TYPE && (!tokcmp (arg2.error.description, "cannot evaluate ;")
				     || !tokcmp (arg2.error.description, "cannot evaluate <-")
				     || !tokcmp (arg2.error.description, "cannot evaluate empty expression")))
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

  return_value.return_signal = false;
  return_value.break_signal  = false;

  return return_value;
}
