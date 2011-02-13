/* sprout.c - FACT model for concurrency. */
#include "FACT.h"

/* @next_tid - holds the next available thread id to be used. */ 
static unsigned long tid;

/* @threads - array of structures that holds all threads being
 * run and their respective queues. It's declared as a pointer 
 * so that it may be dynamically allocated to fit the number
 * of threads being run. Thread ids start with 1, as the 0th
 * thread is the main thread.
 */
static struct thread
{
  bool        remove        ; /* whether or not the thread should be deallocated. */
  bool        exited        ; /* whether or not the thread has returned.          */
  var_t     * return_status ; /* value returned by the thread on exit.            */
  pthread_t   thread_data   ; /* pthread object for the thread.                   */

  /* @queue - variables being passed to the thread. First in
   * first out method is used.
   * @root - points to the next variable to be accessed with
   * the 'queue' BIF. 
   */
  struct queue
  {
    struct queue * next;
    var_t        * value;
  } * root;
} * threads;

/* Threading BIFs and statements:
 *  + sprout ( VARIABLE ) EXPRESSION - create a new thread running EXPRESSION,
 *    setting VARIABLE to the new thread's ID number.
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
  unsigned long index;

  if (!tid)
    {
      /* If the main thread does not exist yet, sprout it. We do not need to
       * initialize the thread_data element as it is not an actual thread.
       */
      tid++;
      threads = better_malloc (sizeof (struct thread));
      threads[0].queue         = NULL  ;
      threads[0].remove        = false ;
      threads[0].exited        = false ;
      threads[0].return_status = NULL  ;
    }
  
  /* Search through all the threads to see if there's one that is ready to
   * be removed and has already exited. We skip the 0th thread as it is
   * the main thread. 
   */
  for (index = 1; index < tid; index++)
    {
      if (threads[index].exited && threads[index].remove)
	break;
    }

  if (index == tid)
    {
      /* Allocate memory for a new thread. */
      tid++;
      threads = better_realloc (threads, sizeof (struct thread) * tid);
    }
  
  threads[index].queue         = NULL  ;
  threads[index].remove        = false ;
  threads[index].exited        = false ;
  threads[index].return_status = NULL  ;
  
