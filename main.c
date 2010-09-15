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

  scope = alloc_func ();
  scope->name = "main";

  for (;;)
    {
      static struct option long_options[] =
	{
	  {"show-heap", no_argument,       &memt,  1},
	  {"hide-heap", no_argument,       &memt,  0},
	  {"stdin",     no_argument,       &cmdln, 1},
	  {"no-stdin",  no_argument,       &cmdln, 0},
	  {"file",      required_argument, 0, 'f'},
	  {0, 0, 0, 0}
	};

      int option_index = 0;

      arg = getopt_long (argc, argv, "mnsdf:", long_options, &option_index);

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
	  line_number = 0;
	  file_open = run_file (scope, optarg);

	  if (file_open.type == ERROR_TYPE)
	    errorman_dump (file_open.error, 0, optarg);

	  break;

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
  GC_INIT ();
  init_std_prims ();
  
  mp_set_memory_functions (&gmp_malloc,
			   &gmp_realloc,
			   &gmp_free);
  process_args (argc, argv);
  GC_gcollect ();
  exit (0);
}
