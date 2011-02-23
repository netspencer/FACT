#include "FACT.h"

/* This is almost deprecated! Almost! */
static struct  _prims
{
  const char * name;
  FACT_t    (* function)(func_t *, word_list);
} primitives [] =
  {
    /* Sorted in ascii order, I guess. */
    { "!["    , return_array },
    { "\""    , new_string   }, /* To be removed and replaced with bcode instruction. */
    { "$"     , run_func     },
    { "&"     , new_scope    },
    { "&&"    , and          },
    { "("     , paren        }, /* DEFINITELY to be removed. */
    { "--"    , negative     },
    { ":"     , in_scope     },
    { "="     , set          },
    { "@"     , liven_func   },
    { "NULL"  , NULL_func    },
    { "def"   , define       },
    { "defunc", defunc       },
    { "lambda", lambda       },
    { "this"  , this         },
    { "up"    , up           },
    { "{"     , lambda_proc  },
    { "||"    , or           },
  };
static int num_of_prims;

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
init_BIFs (func_t *scope)
{
  /* There are certain functions that we either
   * need (such as import) or are better suited
   * being built in than being a standard library
   * functions. Those go in here. This function
   * is loaded once during start-up and then is
   * forgotten. It should NEVER be called twice.
   */
  func_t * ref     ;
  func_t * deref   ;
  func_t * import  ;
  func_t * size_of ; // Actually written as 'sizeof'.
  func_t * get_tid ;

  ref     = add_func (scope, "ref"    );
  deref   = add_func (scope, "deref"  );
  import  = add_func (scope, "import" );
  size_of = add_func (scope, "sizeof" );
  get_tid = add_func (scope, "get_tid");

  ref->args     = get_words (FACT_BIF (ref).arguments    );
  deref->args   = get_words (FACT_BIF (deref).arguments  );
  import->args  = get_words (FACT_BIF (import).arguments );
  size_of->args = get_words (FACT_BIF (sizeof).arguments );
  get_tid->args = get_words (FACT_BIF (get_tid).arguments);

  ref->extrn_func     = FACT_BIF (ref).function;
  deref->extrn_func   = FACT_BIF (deref).function;
  import->extrn_func  = FACT_BIF (import).function;
  size_of->extrn_func = FACT_BIF (sizeof).function;
  get_tid->extrn_func = FACT_BIF (get_tid).function;
}

int
isprim (char *word)
{
  /* isprim - check to see if a token is a valid primitive.
   * This function will be deprecated very soon, but for now
   * is legitamitely (and unfortunately) necessary. It 
   * returns -1 if no primitive is found. Otherwise, it
   * returns the place in the "primitives" struct array where
   * it was found (which is 0 or positive, if you didn't know).
   */
  struct _prims   key;
  struct _prims * result;

  /* Set num_of_prims if it isn't set already. */
  if (!num_of_prims)
    num_of_prims = ((sizeof (primitives)) / sizeof (struct _prims));
  
  key.name = word; 
  result = bsearch (&key, primitives, num_of_prims, sizeof (struct _prims), comp_prims);

  if (result == NULL)
    return -1;
  else
    return result - primitives;
}

FACT_t
run_prim (func_t *scope, word_list expression, int prim_num)
{
  /* run_prim - this function is incredibly simple, and many
   * may question its very existance. Simply put, this will
   * become slightly more complicated in the future, but not 
   * for a little while. Bare with me.
   */
  
  return primitives[prim_num].function (scope, expression);
}

FACT_t
eval_math (func_t *scope, word_list expression, int call_num)
{
  /* eval_math - this is pretty much exactly like run_prim,
   * except the function pointer is different. Also, I
   * could only have used two FACT_t variables, but that
   * would be really confusing.
   */
  FACT_t arg1;
  FACT_t arg2;
  FACT_t return_value;

  /* This is a list of every math call's corresponding
   * function. It's an array of function pointers. Wild!
   */
  static FACT_t (* math_calls[])(FACT_t, FACT_t) =
    {
      not_equal       , // != 0
      mod             , // %  1
      mod_assignment  , // %= 2
      mult            , // *  3
      mult_assignment , // *= 4
      add             , // +  5
      add_assignment  , // += 6
      sub             , // -  7
      sub_assignment  , // -= 8
      divide          , // /  9
      div_assignment  , // /= 10
      less            , // <  11
      less_equal      , // <= 12
      equal           , // == 13
      more            , // >  14
      more_equal      , // >= 15
      bit_xor         , // ^  16
      bit_and         , // `  17
      bit_ior         , // |  18
      combine_arrays  , // ~  19 
    };

  /* Evaluate the two arguments to the math call, and
   * if they are errors, return them.
   */
  if ((arg1 = eval (scope, expression)).type == ERROR_TYPE)
    return arg1;
  if ((arg2 = eval (scope, expression)).type == ERROR_TYPE)
    return arg2;

  return_value = math_calls[call_num] (arg1, arg2);

  /* Since math calls do not get sent a scope to use
   * for error handeling, we have to set it in this
   * function.
   */
  if (return_value.type == ERROR_TYPE)
    return_value.error.scope = scope;

  return return_value;
}
