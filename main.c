#include "common.h"

unsigned int bytes_used = 0;
bool mem_trackopt = false;

void
process_args (int argc, char **argv)
{
  int arg;
  a_type file_open;
  static int memt = false;
  static int cmdln = true;
  func *scope;
  var *inter_argc;
  var *inter_argv;
 
  scope = alloc_func ();
  scope->name = "main";

  file_open = run_file (scope, "stdlib.ft", true);

  if (file_open.type == ERROR_TYPE)
    errorman_dump (file_open.error, 0, optarg);

  for (;;)
    {
      static struct option long_options[] =
	{
	  {"show-heap", no_argument,       &memt,    1},
	  {"hide-heap", no_argument,       &memt,    0},
	  {"stdin",     no_argument,       &cmdln,   1},
	  {"no-stdin",  no_argument,       &cmdln,   0},
	  {"shebang",   required_argument, 0, 'i'},
	  {"file",      required_argument, 0, 'f'},
	  {0, 0, 0, 0}
	};

      int option_index = 0;

      arg = getopt_long (argc, argv, "mnsdi:f:", long_options, &option_index);

      if (arg == -1)
	break;

      switch (arg)
	{
	  
	case 0:
	  if (long_options[option_index].flag != 0)
	    break;
	  break;
	  
	case 'm':
	  memt = true;
	  break;

	case 'n':
	  memt = false;
	  break;

	case 's':
	  cmdln = true;
	  break;

	case 'd':
	  cmdln = false;
	  break;

	case 'f':
	  mem_trackopt = memt;
	  file_open = run_file (scope, optarg, false);

	  if (file_open.type == ERROR_TYPE)
	    errorman_dump (file_open.error, 0, optarg);

	  break;

	case 'i':
	  mem_trackopt = memt;
	  
	  /* parse remaining arguments into variables */
	  inter_argc = add_var (scope, "argc");
	  mpc_set_si (&(inter_argc->data), argc - 2);

	  inter_argv = add_var (scope, "argv");

	  if (argc - 2 <= 1)
	    inter_argv->array_size = 1;
	  else
	    {
	      inter_argv->array_size = argc - 2;
	      inter_argv->array_up = string_array_to_var (argv + 2, "argv", argc - 2);
	    }

	  file_open = run_file (scope, optarg, true);

	  if (file_open.type == ERROR_TYPE)
	    errorman_dump (file_open.error, 0, optarg);

	  exit (0);

	case '?':
	  break;

	default:
	  abort ();
	}
    }

  mem_trackopt = memt;

  if (cmdln)
    shell (scope);
}

int
main (int argc, char **argv)
{
  GC_INIT ();
  init_std_prims ();
  
  mp_set_memory_functions (&gmp_malloc,
			   &gmp_realloc,
			   &gmp_free);
  process_args (argc, argv);
  GC_gcollect ();
  exit (0);
}
