#include "FACT.h"

void
process_args (int argc, char **argv)
{
  int    arg;
  FACT_t file_open;
  
  var_t  *inter_argc;
  var_t  *inter_argv;
  func_t *scope;

  static int cmdln;

  cmdln       = true;
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
          // If we're executing a file, parse the remaining arguments into variables.
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

void *
gmp_realloc_wrapper (void *op1, size_t uop, size_t op2)
{
  FACT_realloc (op1, op2);
}

void
gmp_free_wrapper (void *op1, size_t op2)
{
  // We do not use op2.
  FACT_free (op1);
}

int
main (int argc, char **argv)
{
  // Set exit routines
  atexit (GC_gcollect);
  atexit (close_libs);
  atexit (thread_cleanup);

  // Start the garbage collector
  GC_INIT ();

  // Set up GMP library
  mp_set_memory_functions (&FACT_malloc,
			   &gmp_realloc_wrapper,
			   &gmp_free_wrapper);

  // Start the main thread
  root_thread = FACT_malloc (sizeof (FACT_thread_t));
  root_thread->tid = pthread_self ();
  root_thread->exited  = false;
  root_thread->destroy = false;
  root_thread->next = NULL;
  root_thread->prev = NULL;
  root_thread->root = NULL;
  root_thread->nid  = 0;
  pthread_mutex_init (&root_thread->queue_lock, NULL);

  // Process the arguments and start the interpreter.
  process_args (argc, argv);

  // Exit.
  exit (0);
}
