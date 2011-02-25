/* sprout.c - FACT model for concurrency. */
#include "FACT.h"

bool threading_status;   // True if concurrency is turned on, false otherwise.
FACT_thread_t * threads; // Contains all the thread data.
unsigned long next;      // Value of the next available thread.

unsigned long
FACT_get_tid (pthread_t p1)
{
  /* FACT_get_tid - searches the thread array for the first instance of
   * p1. Returns -1 if p1 is not a valid tid.
   */
  unsigned long i;

  for (i = 0; i < next; i++)
    {
      if (pthread_equal (p1, threads[i].tid))
        return i;
    }

  return -1;
}

struct FACT_pthread_wrap // Wrapper for passing arguments through pthreads.
{
  func_t        * op1;
  word_list       op2;
  unsigned long   op3;
};
  
void *
thread_wrapper (void *args)
{
  // Used as a wrapper between pthreads_create and eval_expression.
  func_t    * scope        ;
  word_list   expression   ;
  unsigned long thread_num ;

  scope = ((struct FACT_pthread_wrap *) args)->op1;
  expression = ((struct FACT_pthread_wrap *) args)->op2;
  thread_num = ((struct FACT_pthread_wrap *) args)->op3;

  reset_ip ();

  threads[thread_num].return_status = eval_expression (scope, expression);
  threads[thread_num].exited = true;

  return (NULL);
}

FACT_t
sprout (func_t * scope, word_list expression)
{
  /* sprout - a statement. Creates a new thread. */
  func_t *top;
  unsigned long i;
  
  struct FACT_pthread_wrap * args;

  expression.syntax++;
  expression.lines++;

  if (!threading_status)
    {
      /* If concurrency has yet to be turned on, our first task should be
       * to create the main thread.
       */
      threads = better_malloc (sizeof (FACT_thread_t));

      threads[0].tid = pthread_self ();
      threads[0].exited  = false;
      threads[0].destroy = false;
      threads[0].root = NULL;

      next = 1;
      threading_status = true;
    }
  
  /* Search through all the threads to see if there's one that is ready to
   * be removed and has already exited. We skip the 0th thread as it is
   * the main thread. 
   */
  for (i = 1; i < next; i++)
    {
      if (threads[i].exited && threads[i].destroy)
	break;
    }
  if (i == next)
    {
      // Allocate memory for a new thread.
      next++;
      threads = better_realloc (threads, sizeof (FACT_thread_t) * next);
    }

  // Find the topmost scope.
  for (top = scope; top->up != NULL; top = top->up);
  
  args = better_malloc (sizeof (struct FACT_pthread_wrap));

  args->op1 = alloc_func (); // Create a new scope.
  args->op1->up = top;
  args->op2 = expression;
  args->op3 = i;

  pthread_create (&(threads[i].tid), NULL, thread_wrapper, (void *) args);

  return FACT_get_ui (i);
}

FACT_DEFINE_BIF (get_tid, "")
{
  // Get the current thread number.
  return FACT_get_ui ((threading_status)
                      ? FACT_get_tid (pthread_self ())
                      : 0);
}

FACT_DEFINE_BIF (get_thread_status, "def tid")
{
  /**
   * get_thread_status - returns true if a thread has exited and
   * false otherwise.
   *
   * @tid: The thread id of the thread to test.
   */
  unsigned long tid;

  tid = mpc_get_ui ((get_var (scope, "tid"))->data); // Convert the var_t to a ulong.
  if (tid < 0 || tid >= next) // If the tid is invalid, throw an error.
    errorman_throw_catchable (scope, "invalid thread id");

  return FACT_get_ui (threads[tid].exited);
}
