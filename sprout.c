/* sprout.c - FACT model for concurrency. */
#include "FACT.h"

FACT_thread_t *
FACT_get_curr_thread ( void )
{
  /**
   * FACT_get_tid - searches the thread list for the first instance of
   * pthread_self (). If the current thread is not allocated we return NULL. 
   */
  FACT_thread_t *curr;

  curr = root_thread;
  do
    {
      if (pthread_equal (pthread_self (), curr->tid))
        return curr;
    }
  while ((curr = curr->next) != NULL);

  return NULL;
}

void *
thread_wrapper (void *arg)
{
  /**
   * thread_wrapper - Used as a wrapper between pthreads_create and
   * eval_expression. Also creates the scope for the thread.
   */
  syn_tree_t exp;

  func_t        *scope;
  FACT_thread_t *curr;

  // Get the expression and the current tid.
  exp = *((syn_tree_t *) arg);

  do
    curr = FACT_get_curr_thread ();
  while (curr == NULL);

  // Create the new scope and initialize its BIFs.
  scope = alloc_func ();
  init_BIFs (scope);

  curr->return_status = eval_expression (scope, make_syn_tree (exp.syntax, exp.lines));
  curr->exited = true;

  // if stdout is turned on...
  if (curr->return_status.type == ERROR_TYPE)
    errorman_dump (curr->return_status.error);

  pthread_mutex_destroy (&curr->queue_lock);
  pthread_exit (NULL);
}


FACT_t
sprout (func_t * scope, syn_tree_t expression)
{
  /**
   * sprout - Create a new thread and manage the threads structure. 
   * returns the id of the thread created.
   */
  syn_tree_t    *arg;
  FACT_thread_t *curr;
  FACT_thread_t *temp;

  static pthread_mutex_t list_lock = PTHREAD_MUTEX_INITIALIZER;
    
  expression.syntax += get_ip ();
  arg = better_malloc (sizeof (syn_tree_t));
  *arg = expression;

  // Wait until we have control of the list
  while (pthread_mutex_trylock (&list_lock) == EBUSY); // Do nothing
  
  for (curr = root_thread; curr->next != NULL; curr = curr->next)
    {
      if (curr->exited && curr->destroy)
        break;
    }

  if (curr->exited && curr->destroy)
    {
      // Use a pre-existing thread.
      curr->ip = 0;
      curr->exited = false;
      curr->destroy = false;
      curr->root = NULL;
    }
  else
    {
      // Allocate memory for a new thread.
      temp = FACT_malloc (sizeof (FACT_thread_t));
      pthread_mutex_init (&temp->queue_lock, NULL);
      temp->ip = 0;
      temp->nid = curr->nid + 1;
      temp->exited = false;
      temp->destroy = false;
      temp->root = NULL;
      temp->next = NULL;
      temp->prev = curr;
      curr->next = temp;
      curr = curr->next;
    }

  pthread_create (&(curr->tid), NULL, thread_wrapper, (void *) arg);
  pthread_mutex_unlock (&list_lock);
  return FACT_get_ui (curr->nid);
}

FACT_DEFINE_BIF (get_tid, NOARGS)
{
  /**
   * get_tid - Get the current thread number.
   */
  return FACT_get_ui (FACT_get_curr_thread ()->nid);
}

FACT_DEFINE_BIF (get_thread_status, "def tid")
{
  /**
   * get_thread_status - returns true if a thread has exited and
   * false otherwise.
   *
   * @tid: The thread id of the thread to test.
   */
  unsigned long i;
  unsigned long tid;
  FACT_thread_t *curr;

  tid = mpc_get_ui ((get_var (scope, "tid"))->data); // Convert the var_t to a ulong.
  if (tid < 0) // If the tid is invalid, throw an error.
    FACT_ret_error (scope, "invalid thread id");

  curr = root_thread;
  for (i = 0; i < tid; i++)
    {
      curr = curr->next;
      if (curr == NULL)
        FACT_ret_error (scope, "invalid thread id");
    }
  
  return FACT_get_ui (i);
}

FACT_DEFINE_BIF (queue_size, NOARGS)
{
  /**
   * queue_size - get the size of the queue of the current thread.
   */
  int count;
  FACT_thread_t *curr_thread;
  struct queue *curr_queue;

  curr_thread = FACT_get_curr_thread ();

  while (pthread_mutex_trylock (&curr_thread->queue_lock) == EBUSY);

  count = 0;
  for (curr_queue = curr_thread->root; curr_queue != NULL; curr_queue = curr_queue->next)
    count++;

  pthread_mutex_unlock (&curr_thread->queue_lock);

  return FACT_get_ui (count);
}

FACT_DEFINE_BIF (pop, NOARGS)
{
  /**
   * pop - pop the first value in the queue of a thread, returning 
   * and removing it. If there are no values in the queue, it throws
   * an error. 
   */
  FACT_t return_value;
  FACT_thread_t *curr;

  curr = FACT_get_curr_thread ();
  return_value.type = VAR_TYPE;

  // Wait for our turn.
  while (pthread_mutex_trylock (&curr->queue_lock) == EBUSY);

  if (curr->root == NULL)
    FACT_ret_error (scope, "current thread's queue is empty");

  // Set the return value.
  return_value.v_point = curr->root->value;

  // Remove the node.
  curr->root = curr->root->next;

  // Give up control.
  pthread_mutex_unlock (&curr->queue_lock);

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
  unsigned long tid;

  var_t         *msg;
  FACT_thread_t *dest;
  struct queue  *curr;
  struct queue  *set;

  // Get the arguments
  tid = mpc_get_ui ((get_var (scope, "tid"))->data);
  msg = get_var (scope, "msg");

  // If the tid is obviously invalid, throw an error.
  if (tid < 0)
    FACT_ret_error (scope, "invalid thread id");

  for (dest = root_thread; dest != NULL; dest = dest->next)
    {
      if (dest->nid == tid)
        break;
    }

  if (dest == NULL)
    FACT_ret_error (scope, "invalid thread id");
  else if (dest->exited)
    FACT_ret_error (scope, "thread has exited");

  // Wait for control.
  while (pthread_mutex_trylock (&dest->queue_lock) == EBUSY);

  if (dest->root == NULL)
    {
      dest->root = better_malloc (sizeof (struct queue));
      set = dest->root;
    }
  else
    {
      for (curr = dest->root; curr->next != NULL; curr = curr->next);
      curr->next = better_malloc (sizeof (struct queue));
      set = curr->next;
    }

  set->next = NULL;
  set->value = msg;

  pthread_mutex_unlock (&dest->queue_lock);
  return FACT_get_ui (0);
} 

void
thread_cleanup ( void )
{
  /* This function is passed to atexit and is used to clean up all
   * the running threads. 
   */
  FACT_thread_t *curr;

  // Skip the main thread.
  for (curr = root_thread->next; curr != NULL; curr = curr->next)
    pthread_join (curr->tid, NULL);
}
