#include "common.h"

void
process_args (int argc, char **argv)
{
  int          arg;
  var_t      * inter_argc;
  var_t      * inter_argv;
  FACT_t       file_open;  
  func_t     * scope;
  static int   cmdln = true;

  set_bytes_used (0);
 
  scope       = alloc_func ();
  scope->name = "main";
  init_BIFs (scope);

  file_open = run_file (scope, "/etc/FACT/include/stdlib.ft", true);

  if (file_open.type == ERROR_TYPE)
    errorman_dump (file_open.error);

  for (;;)
    {
      static struct option long_options[] =
	{
	  { "stdin"    , no_argument       , &cmdln , 1   } ,
	  { "no-stdin" , no_argument       , &cmdln , 0   } ,
	  { "shebang"  , required_argument , 0      , 'i' } ,
	  { "file"     , required_argument , 0      , 'f' } ,
	  { 0          , 0                 , 0      , 0   } ,
	};

      int option_index = 0;

      arg = getopt_long (argc, argv, "sdi:f:", long_options, &option_index);

      if (arg == -1)
	break;

      switch (arg)
	{
	  
	case 0:
	  if (long_options[option_index].flag != 0)
	    break;
	  break;

	case 's':
	  cmdln = true;
	  break;

	case 'd':
	  cmdln = false;
	  break;

	case 'f':
	  file_open = run_file (scope, optarg, false);

	  if (file_open.type == ERROR_TYPE)
	    errorman_dump (file_open.error);

	  break;

	case 'i':
	  /* ---- Parse remaining arguments into variables ---- */
	  inter_argc = add_var (scope, "argc");
	  mpc_set_si (&(inter_argc->data), argc - 2);

	  inter_argv = add_var (scope, "argv");

	  if (argc - 2 <= 1)
	    mpz_set_ui (inter_argv->array_size, 1);
	  else
	    {
	      mpz_set_ui (inter_argv->array_size, argc - 2);
	      inter_argv->array_up = string_array_to_var (argv + 2, "argv", argc - 2);
	    }

	  file_open = run_file (scope, optarg, true);

	  if (file_open.type == ERROR_TYPE)
	    errorman_dump (file_open.error);
	  exit (0);

	case '?':
	  break;

	default:
	  abort ();
	}
    }

  if (cmdln)
    shell (scope);
}

int
main (int argc, char **argv)
{
  atexit (GC_gcollect);
  atexit (close_libs);

  GC_INIT ();
  
  mp_set_memory_functions (&FACT_malloc,
			   &FACT_realloc,
			   &FACT_free);
  process_args (argc, argv);
  exit (0);
}
