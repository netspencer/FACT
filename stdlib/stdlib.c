#include "stdlib.h"

FACT_t
ft_putchar (func_t *scope)
{
  var_t *to_put;

  to_put = get_var (scope, "char");
  putchar (var_get_si (to_put));

  return FACT_get_ui (0);
}
    
FACT_t
throw_error (func_t *scope)
{
  FACT_t return_value;
  char  *description;
  var_t *to_throw;

  to_throw = get_var (scope, "description");

  description = array_to_string (to_throw);
  return_value.type = ERROR_TYPE;
  return_value.error.scope = scope->up;
  return_value.error.description = description;

  return return_value;
}

struct elements MOD_MAP [] =
  {
    { "fopen"       , "def filename, def mode" , &open_file     },
    { "fclose"      , "defunc file_object"     , &close_file    },
    { "fgetc"       , "defunc file_object"     , &get_char_file },
    { "run"         , "def filename, defunc in", &run_file_soft },
    { "run_loud"    , "def filename, defunc in", &run_file_loud },
    { "putchar"     , "def char"               , &ft_putchar    },
    // { "lock"        , "defunc search, def name", &lock_object   },
    { "throw"       , "def description"        , &throw_error   },
    { "get_arg_type", "->"                     , &get_arg_type  },
    { "get_arg_num" , ""                       , &get_arg_num   },
    { "get_arg"     , "def num"                , &get_arg       },
    { "int"         , "def op"                 , &var_to_int    },
    { "random"      , ""                       , &simple_rand   },
    { NULL          , NULL                     , NULL           },
  };
