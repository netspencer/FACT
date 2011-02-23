/* sprout.c - FACT model for concurrency. */
#include "FACT.h"

bool            threading_status ; // True if we are threading, false otherwise. 
jmp_buf         station          ; // Jump buffer that jumps us back to the station.
thread_t      * threads          ; // All the thread data.
unsigned long   tid              ; // Holds the next available thread id to be used.
unsigned long   curr_tid         ; // Holds the current thread's id.

void
scheduler ( void )
{
  /**
   * scheduler: rotates between all the threads, executing one expression per
   * each. 
   */
  unsigned long original;

  // If this is the first thread created, set the station jmp_buf and return.
  if (!setjmp (station))
    return;

  // Main loop.
 loop:

  /* Increment the current thread by one, accounting for boundries and 
   * exited/returned threads. We use the variable 'original' to check
   * if we've hit an infinite loop.
   */
  original = curr_tid;
  do
    {
      curr_tid = (++curr_tid) % tid;
      
      if (curr_tid == original)
        {
          // Jump back to the main thread if everything has exited.
          curr_tid = 0;
          break;
        }
    }
  while (threads[curr_tid].exited);
  
  set_ip (threads[curr_tid].ip);
  
  if (threads[curr_tid].virgin)
    {
      threads[curr_tid].virgin = false;
      threads[curr_tid].return_status = eval_expression (threads[curr_tid].scope, threads[curr_tid].expression);
      threads[curr_tid].exited = true;

      goto loop; // Go back to the beginning.
    }

  longjmp (threads[curr_tid].back, -1); // Jump back to the thread.

  // NOT REACHED.

  fprintf (stderr, "End of function 'scheduler' reached, aborting.\n");
  abort ();
}
            
/**
 * Threading BIFs and statements:
 *  + sprout EXPRESSION - create a new thread running EXPRESSION, returning
 *    the sprouted thread's tid.
 *  + $(get_tid) - get the id of the current thread.
 *  + $(send_to_thread, def tid, def var) - send variable var to tid's queue.  
 *  + $(get_queue_length) - return the number of items in a thread's queue.
 *  + $(get_queue) - return the next available variable in the current thread's
 *    queue.
 */

FACT_t
sprout (func_t * scope, word_list expression)
{
  /* sprout - a statement. Creates a new thread. */
  static bool not_first;
  unsigned long i;

  expression.syntax++;
  expression.lines++;

  if (!not_first)
    {
      /* If the main thread does not exist yet, sprout it. We do not need to
       * initialize the thread_data element as it is not an actual thread.
       */
      threads = better_malloc (sizeof (struct thread));
      
      threads[0].remove     = false ;
      threads[0].exited     = false ;
      threads[0].scope      = scope ;
      threads[0].root       = NULL  ;

      scheduler ();
      
      tid      = 1;
      curr_tid = 0;
      // The mother thread sets its own expression and ip data.
    }
  
  /* Search through all the threads to see if there's one that is ready to
   * be removed and has already exited. We skip the 0th thread as it is
   * the main thread. 
   */
  for (i = 1; i < tid; i++)
    {
      if (threads[i].exited && threads[i].remove)
	break;
    }

  if (i == tid)
    {
      /* Allocate memory for a new thread. */
      tid++;
      threads = better_realloc (threads, sizeof (struct thread) * tid);
    }

  threads[0].remove     = false;
  threads[0].exited     = false;
  threads[0].scope      = alloc_func ();
  threads[0].expression = expression;
  threads[0].ip         = 0;
  threads[0].root       = NULL;

  return FACT_get_ui (i);
}

FACT_DEFINE_BIF (get_tid, "")
{
  return FACT_get_ui (curr_tid);
}
