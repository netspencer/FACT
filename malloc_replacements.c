/* This file is part of FACT.
 *
 * FACT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FACT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FACT. If not, see <http://www.gnu.org/licenses/>.
 */

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
