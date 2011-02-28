/* sprout.c - FACT model for concurrency. */
#include "FACT.h"

unsigned long next; // Points to the next available thread.

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

void *
thread_wrapper (void *arg)
{
  /* thread_wrapper - Used as a wrapper between pthreads_create and
   * eval_expression. Also creates the scope for the thread.
   */
  char      ** new_tree   ;
  func_t     * scope      ;
  word_list    expression ;

  unsigned long i;
  unsigned long thread_num;

  // Get the expression and the current tid.
  expression = *((word_list *) arg);
  thread_num = FACT_get_tid (pthread_self ());

  /* Reset the instruction pointer in case this thread is being
   * re-used.
   */
  reset_ip (); 

  // Create the new scope and initialize its BIFs.
  scope = alloc_func ();
  init_BIFs (scope);

  // Create a new syntax tree.
  new_tree = NULL;
  for (i = 0; expression.syntax[i] != NULL; i++)
    {
      new_tree = better_realloc (new_tree, sizeof (char *) * (i + 2)); // The + 2 is for the NULL terminator.
      new_tree[i] = expression.syntax[i];
    }
  new_tree[i] = NULL;
  expression.syntax = new_tree;

  threads[thread_num].return_status = eval_expression (scope, expression);
  threads[thread_num].exited = true;

  // if stdout is turned on...
  if (threads[thread_num].return_status.type == ERROR_TYPE)
    errorman_dump (threads[thread_num].return_status.error);

  pthread_exit (NULL);
}

FACT_t
sprout (func_t * scope, word_list expression)
{
  /* sprout - a statement. Creates a new thread. */
  word_list * arg;
  unsigned long i;

  expression.syntax += get_ip ();
  expression.lines  += get_ip ();

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

      pthread_mutex_init (&threads[0].queue_safe, NULL);

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

  threads[i].root = NULL;
  pthread_mutex_init (&threads[i].queue_safe, NULL);

  arg = better_malloc (sizeof (word_list));
  *arg = expression;
  pthread_create (&(threads[i].tid), NULL, thread_wrapper, (void *) arg);

  return FACT_get_ui (i);
}

FACT_DEFINE_BIF (get_tid, NOARGS)
{
  /**
   * get_tid - Get the current thread number.
   */
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

FACT_DEFINE_BIF (queue_size, NOARGS)
{
  /**
   * queue_size - get the size of the queue of the current thread.
   */
  unsigned long   count;
  struct queue  * i;
  
  if (!threading_status)
    return FACT_get_ui (0);

  count = 0;
  for (i = threads[FACT_get_tid (pthread_self ())].root; i != NULL; i = i->next)
    count++;

  return FACT_get_ui (count);
}

FACT_DEFINE_BIF (pop, NOARGS)
{
  /**
   * pop - pop the first value in the queue of a thread, returning 
   * and removing it. If there are no values in the queue, it throws
   * an error. This function is completely reentrant.
   */
  FACT_t return_value;
  FACT_thread_t *current_thread;

  current_thread = threads + FACT_get_tid (pthread_self ());
  return_value.type = VAR_TYPE;

  // Wait until the mutex of the thread is unlocked, then lock it.
  while (pthread_mutex_trylock (&current_thread->queue_safe) == EBUSY)
    ; // Do nothing.
  
  if (current_thread->root == NULL)
    errorman_throw_catchable (scope, "current thread's queue is empty");

  // Pop the first first value and set root to the next value.
  return_value.v_point = current_thread->root->value;
  current_thread->root = current_thread->root->next;

  pthread_mutex_unlock (&current_thread->queue_safe);

  return return_value;
}

FACT_DEFINE_BIF (send, "def tid, def msg")
{
  /**
   * send - send a message to a thread. Throws an error if the tid
   * is invalid or the thread has exited.
   *
   * @tid: Thread id of the thread to send to.
   * @msg: Message to send to the thread.
   */
  var_t         * msg            ;
  unsigned long   tid            ;
  FACT_thread_t * current_thread ;
  struct queue  * i              ;

  // Get the arguments
  tid = mpc_get_ui ((get_var (scope, "tid"))->data);
  msg = get_var (scope, "msg");

  if (threads == NULL)
    errorman_throw_catchable (scope, "there was an error somewhere");
  if (tid < 0 || tid >= next || threads[tid].exited) // If the tid is invalid, throw an error.
    errorman_throw_catchable (scope, "invalid thread id");

  current_thread = threads + tid;
  
  // Wait until the mutex of the thread is unlocked, then lock it.
  while (pthread_mutex_trylock (&current_thread->queue_safe) == EBUSY)
    ; // Do nothing.

  // If the queue is empty, set root.
  if (current_thread->root == NULL)
    {
      current_thread->root = better_malloc (sizeof (struct queue));
      current_thread->root->next  = NULL;
      current_thread->root->value = msg;
    }
  else
    {
      // Otherwise, move to the second to last value in the stack.
      for (i = current_thread->root; i->next != NULL; i = i->next)
        ; // Do nothing.
      i->next = better_malloc (sizeof (struct queue));
      i->next->next  = NULL;
      i->next->value = msg;
    }

  pthread_mutex_unlock (&current_thread->queue_safe);
  return FACT_get_ui (0);
} 

void
thread_cleanup ( void )
{
  /* This function is passed to atexit and is used to clean up all
   * the running threads. Really simple.
   */
  unsigned long i;

  for (i = 1; i < next; i++) // Skip the main thread.
    pthread_join (threads[i].tid, NULL);
}
