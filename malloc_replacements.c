#include "malloc_replacements.h"

/* These functions are used to replace
 * the current malloc functions with
 * ones that are a bit more safe (abort
 * on NULL pointer return, etc.)
 */

static unsigned int bytes_used;

void
set_bytes_used (unsigned int op)
{
  bytes_used = op;
}

unsigned int
get_bytes_used ( void )
{
  return bytes_used;
}

/* Here's a rough idea of how these
 * functions work: bytes_used is a counter
 * of all the bytes that have been alloc'd
 * in the program. When the counter reaches
 * a certain point, garbage collection is
 * forced and the counter is reset. Simple,
 * I'm not sure whether it's effective or
 * not. Probably not.
 */

void *
FACT_malloc (size_t alloc_size)
{
  void * temp_pointer;

#ifdef GATES_OF_VALGRIND
  temp_pointer = malloc (alloc_size);

  if (temp_pointer == NULL)
    {
      fprintf (stderr, "Could not allocate block of size %d, exiting.\n", (int) alloc_size);
      abort ();
    }
  
#else
  
  if (bytes_used >= MAX_BYTES)
    {
#ifdef MEM_DEBUG
      printf ("bytes_used exceeds MAX_BYTES\n"
	      "Current-heap = %d\n", (int) GC_get_heap_size ());
#endif
      GC_gcollect ();
#ifdef MEM_DEBUG
      printf ("Heap-after-collect = %d\n", (int) GC_get_heap_size ());
#endif
      bytes_used = 0;
    }
    
  temp_pointer = GC_malloc (alloc_size);
  
  if (temp_pointer == NULL)
      {
      /* If GC returns a NULL pointer, garbage collection is enforced
       * in order to free up as much space as possible. Then,
       * allocation is tried one more time.
       */
      GC_gcollect ();
      temp_pointer = GC_malloc (alloc_size);

      if (temp_pointer == NULL)
	{
	  fprintf (stderr, "Could not allocate block of size %d, exiting.\n", (int) alloc_size);
	  exit (EXIT_FAILURE);
	}
    }
#endif

  return temp_pointer;
}

void *
FACT_realloc (void *to_resize, size_t alloc_size, size_t new_size)
{
  void * temp_pointer;

#ifdef GATES_OF_VALGRIND
  temp_pointer = realloc (to_resize, new_size);

  if (temp_pointer == NULL)
    {
      fprintf (stderr, "Could not allocate block of size %d, exiting.\n", (int) alloc_size);
      abort ();
    }

#else

  if ((int) GC_get_heap_size () == 0)
    GC_gcollect ();

  if (bytes_used >= MAX_BYTES)
    {
#ifdef MEM_DEBUG
      printf ("bytes_used exceeds MAX_BYTES\n"
	      "Current-heap = %d\n", (int) GC_get_heap_size ());
#endif
      GC_gcollect ();
#ifdef MEM_DEBUG
      printf ("Heap-after-collect = %d\n", (int) GC_get_heap_size ());
#endif
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
#endif
  
  return temp_pointer;
}

void
FACT_free (void *to_free, size_t useless)
{
#ifdef GATES_OF_VALGRIND
  free (to_free);
#else
  GC_free (to_free);
#endif
}
