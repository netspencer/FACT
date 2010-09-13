#include "gmp_mallocs.h"


void *gmp_malloc (size_t alloc_size)
{
  void *temp_pointer;

  if (mem_trackopt)
    printf("Heap size = %d\n", (int) GC_get_heap_size());

  temp_pointer = GC_malloc (alloc_size);

  if (temp_pointer == NULL)
    {
      /*
	if GC returns a NULL pointer, garbage collection is enforced
	in order to free up as much space as possible. Then,
	allocation is tried one more time.
      */
      GC_gcollect ();
      temp_pointer = GC_malloc (alloc_size);

      if (temp_pointer == NULL)
	{
	  fprintf (stderr, "Could not allocate block of size %d, exiting.\n", (int) alloc_size);
	  exit (EXIT_FAILURE);
	}
    }

  return temp_pointer;
}

void *gmp_realloc (void *to_resize, size_t alloc_size, size_t new_size)
{
  void *temp_pointer;

  if (mem_trackopt)
    printf("Heap size = %d\n", (int) GC_get_heap_size());

  if (bytes_used >= MAX_BYTES)
    {
      GC_gcollect ();
      bytes_used = 0;
    }

  temp_pointer = GC_realloc (to_resize, new_size);

  bytes_used += alloc_size;

  if (temp_pointer == NULL)
    {
      GC_gcollect ();
      temp_pointer = GC_realloc (to_resize, new_size);

      if (temp_pointer == NULL)
	{
	  fprintf (stderr, "Could not allocate block of size %d, exiting.\n", (int) alloc_size);
	  exit (EXIT_FAILURE);
	}
    }
  
  return temp_pointer;
}

void gmp_free (void *to_free, size_t unneeded) { GC_free (to_free); }

