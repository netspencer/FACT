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
  pthread_t tid;          // Thread id.
  FACT_t return_status;   // Value returned by thread.
  bool exited;            // True if the thread has returned.
  bool destroy;           // True if the thread is set to be freed from memory.

  struct queue            // The variables being passed to the thread.
  {         
    struct queue * next;  // Points to the next value in the queue.
    var_t        * value; // Physical value.
  } * root;               // Points to the first unread value in the queue.
} FACT_thread_t;

//////////////////////
// Global variables.
//////////////////////

extern bool threading_status;
extern FACT_thread_t * threads;

/////////////////////////
// Function prototypes.
/////////////////////////

FACT_INTERN_FUNC (unsigned long) FACT_get_tid (pthread_t);
FACT_INTERN_FUNC (FACT_t) sprout (func_t *, word_list);

//////////////////////////////////
// Threading built in functions. 
//////////////////////////////////

FACT_EXTERN_BIF (get_tid);
FACT_EXTERN_BIF (get_thread_status);

#endif
