#include "common.h"

unsigned int bytes_used = 0;
bool mem_trackopt = false;

int main (int argc, char **argv)
{
  int c;
  
  while ((c = getopt (argc, argv, "m")) != -1)
    switch (c)
      {
      case 'm':
	printf ("Heap size printing enable.\n"
		"The heap size will be printed on every allocation.\n");
	mem_trackopt = true;
	break;
      default:
	abort ();
      }
  
  GC_INIT ();
  //GC_enable_incremental ();
  
  mp_set_memory_functions (&gmp_malloc,
			   &gmp_realloc,
			   &gmp_free);
  shell ();
  GC_gcollect ();
  exit (0);
}
