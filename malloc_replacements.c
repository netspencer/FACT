#include "FACT.h"

void *
FACT_malloc (size_t alloc_size)
{
  void *temp_pointer;
  
  temp_pointer = GC_malloc (alloc_size);

  if (temp_pointer == NULL)
    {
      fprintf (stderr, "Could not allocate block of size %lu, exiting.\n", (unsigned long) alloc_size);
      abort ();
    }
  
  return temp_pointer;
}

void *
FACT_realloc (void *to_resize, size_t new_size)
{
  void * temp_pointer;

  temp_pointer = GC_realloc (to_resize, new_size);

  if (temp_pointer == NULL)
    {
      fprintf (stderr, "Could not allocate block of size %lu, exiting.\n", (unsigned long) new_size);
      abort ();
    }
  
  return temp_pointer;
}

void
FACT_free (void *to_free)
{
  GC_free (to_free);
}
