#include "interpreter.h"

struct { 
  const char *name;
  a_type (*function)(func *, char **);
} primitives[] = {
  {"=", set},
  {"(", paren},
  {"{", lambda_proc},
  {"def", define},
  {"defunc", defunc},
  {"[", return_array},
  {"sizeof", size_of},
  {"@", add_func},
  {"$", run_func},
  {"&", new_scope},
  {":", in_scope},
  {"\"", new_string},
  {"printc", print_character},
  {"getc", input_character},
  {"printv", print_var},
};

struct { 
  const char *name;
  a_type (*function)(a_type, a_type);
} math_calls[] = {
  {"+", add},
  {"-", sub},
  {"*", mult},
  {"/", divide},
  {"%", mod},
  {"eq", equal},
  {"nq", not_equal},
  {">", more},
  {"<", less},
  {"meq", more_equal},
  {"leq", less_equal},
  {"and", and},
  {"or", or},
};

int isprim (char *word)
{
  int num_of_prims;
  int pos;

  num_of_prims = ((sizeof primitives) / (sizeof primitives[0]));

  for (pos = 0; pos < num_of_prims; pos++)
    {
      if (strcmp (primitives[pos].name, word) == 0)
	return pos;
    }

  return -1;
}

int ismathcall (char *word)
{
  int num_of_math_prims;
  int pos;

  num_of_math_prims = ((sizeof math_calls) / (sizeof math_calls[0]));

  for (pos = 0; pos < num_of_math_prims; pos++)
    {
      if (strcmp (math_calls[pos].name, word) == 0)
        return pos;
    }

  return -1;
}

a_type runprim (func *scope, char **words)
{
  a_type return_value;
  int prim_num;

  prim_num = isprim (words[0]);

  return_value = primitives[prim_num].function(scope, words + 1);

  if (prim_num != 2)
    return_value.isret = false;
  
  return return_value;
}

a_type eval_math (func *scope, char **words)
{
  a_type arg1;
  a_type arg2;
  int pos;
  a_type return_value, error;

  arg1 = eval (scope, words + 1);
  arg2 = eval (scope, words + 2);

  /*
  words[1] = words[3];
  words[2] = words[4];
  */

  error.type = ERROR_TYPE;
  error.error.function = "eval_math";
  error.error.scope = scope;
  
  if (arg1.type != VAR_TYPE || arg2.type == FUNCTION_TYPE)
    {
      error.error.error_code = INVALPRIM;
      return error;
    }

  return_value = math_calls[ismathcall (words[0])].function (arg1, arg2);

  if (return_value.type == ERROR_TYPE)
    return_value.error.scope = scope;

  for (pos = 1; words[pos] != NULL; pos++)
    words[pos] = words[pos + 2];

  return_value.isret = false;

  return return_value;
}
