#include "FACT.h"

void *
FACT_malloc (size_t alloc_size)
{
  void * temp_pointer;
  
#ifdef VALGRIND
  temp_pointer = malloc (alloc_size);
#else
  temp_pointer = GC_malloc (alloc_size);
#endif // VALGRIND

  if (temp_pointer == NULL)
    {
      fprintf (stderr, "Could not allocate block of size %lu, exiting.\n", (unsigned long) alloc_size);
      abort ();
    }
  
  return temp_pointer;
}

void *
FACT_realloc (void *to_resize, size_t alloc_size, size_t new_size)
{
  void * temp_pointer;

#ifdef VALGRIND
  temp_pointer = realloc (to_resize, new_size);
#else
  temp_pointer = GC_realloc (to_resize, new_size);
#endif // VALGRIND

  if (temp_pointer == NULL)
    {
      fprintf (stderr, "Could not allocate block of size %lu, exiting.\n", (unsigned long) alloc_size);
      abort ();
    }
  
  return temp_pointer;
}

void
FACT_free (void *to_free)
{
#ifdef VALGRIND
  free (to_free);
#else
  GC_free (to_free);
#endif // VALGRIND
}
