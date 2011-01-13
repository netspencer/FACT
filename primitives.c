#include "primitives.h"

struct         _prims
{
  int          prim_num;
  const char * name;
  FACT_t    (* function)(func_t *, word_list);
};
static int             num_of_prims = 0;
static struct _prims * primitives   = NULL;

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
  func_t * import;
  
  add_prim ("--"    , negative    );
  add_prim ("="     , set         );
  add_prim ("("     , paren       );
  add_prim ("{"     , lambda_proc );
  add_prim ("!["    , return_array);
  add_prim ("@"     , liven_func  );
  add_prim ("$"     , run_func    );
  add_prim ("&"     , new_scope   );
  add_prim (":"     , in_scope    );
  add_prim ("\""    , new_string  );
  add_prim ("&&"    , and         );
  add_prim ("||"    , or          );
  add_prim ("def"   , define      );
  add_prim ("defunc", defunc      );
  add_prim ("sizeof", size_of     ); /* Probably going to remove. */
  add_prim ("lambda", lambda      );
  add_prim ("up"    , up          );
  add_prim ("this"  , this        );
  add_prim ("NULL"  , NULL_func   );
  import             = add_func (scope, "import");
  import->args       = get_words ("def path "); /* the extra space is required for parsing reasons. */
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

  /* This is a list of every math call's corresponding
   * function. It's an array of function pointers. Wild!
   */
  static FACT_t (* math_calls[])(FACT_t, FACT_t) =
    {
      combine_arrays  , // ~ 0
      add             , // + 1
      sub             , // - 2
      mult            , // * 3
      divide          , // / 4
      mod             , // % 5
      bit_and         , // ` 6
      bit_xor         , // ^ 7
      bit_ior         , // | 8
      add_assignment  , // += 9
      sub_assignment  , // -= 10
      mult_assignment , // *= 11
      div_assignment  , // /= 12
      mod_assignment  , // %= 13
      equal           , // == 14
      not_equal       , // != 15
      more            , // > 16
      less            , // < 17
      more_equal      , // >= 18
      less_equal      , // <= 19
    }; 

  arg1 = eval (scope, expression);
  arg2 = eval (scope, expression);
  
  if (arg1.type == ERROR_TYPE)
    return arg1;
  if (arg2.type == ERROR_TYPE)
    return arg2;

  return_value = math_calls[call_num] (arg1, arg2);

  if (return_value.type == ERROR_TYPE)
    return_value.error.scope = scope;

  return_value.return_signal = false;
  return_value.break_signal  = false;

  return return_value;
}
