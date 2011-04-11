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

#ifndef SPROUT_H_
# define SPROUT_H_

#include "FACT.h"

////////////////////
// Typedefs/enums.
////////////////////

/* Threads - array of structures that holds all threads being
 * run and their respective queues. It's declared as a pointer 
 * so that it may be dynamically allocated to fit the number
 * of threads being run. Thread ids start with 1, as the 0th
 * thread is the main thread.
 */
typedef struct thread
{
  bool            exited;        // True if the thread has returned.
  bool            destroy;       // True if the thread is set to be deallocated.
  FACT_t          return_status; // Value returned by thread.
  pthread_t       tid;           // Thread id.
  unsigned long   ip;            // Thread's instruction pointer.
  unsigned long   nid;           // Numeric value that represents the thread. 
  pthread_mutex_t queue_lock;    // Prevents queue race conditions.

  struct thread *next;           // Points to the next thread in the linked list.
  struct thread *prev;           // Points to the previous thread in the list.
  struct queue                   // The variables being passed to other threads.
  {
    struct queue *next;          // Points to the next value in the queue.
    var_t *value;                // Physical value.
  } *root;                       // Points to the first unread value in the queue.
} FACT_thread_t;

//////////////////////
// Global variables.
//////////////////////

FACT_thread_t *root_thread; // Contains all the thread data.

/////////////////////////
// Function prototypes.
/////////////////////////

FACT_INTERN_FUNC (FACT_thread_t *) FACT_get_curr_thread (void);
FACT_INTERN_FUNC (FACT_t) sprout (func_t *, syn_tree_t);
FACT_INTERN_FUNC (void) thread_cleanup (void);

//////////////////////////////////
// Threading built in functions. 
//////////////////////////////////

FACT_EXTERN_BIF (get_tid);
FACT_EXTERN_BIF (get_thread_status);
FACT_EXTERN_BIF (queue_size);
FACT_EXTERN_BIF (pop);
FACT_EXTERN_BIF (send);

#endif
