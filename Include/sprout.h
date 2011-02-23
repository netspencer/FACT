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
  bool            virgin        ; // Whether or not the thread has been run before.
  bool            remove        ; // Whether or not the thread should be deallocated.
  bool            exited        ; // Whether or not the thread has returned.
  FACT_t          return_status ; // Value returned by the thread on exit.
  func_t        * scope         ; // Scope of the thread.
  jmp_buf         back          ; // Buffer to jump back to.
  word_list       expression    ; // Expression to be evaluated in the thread.
  unsigned long   ip            ; // The instruction pointer of the expression.
  struct queue                    // The variables being passed to the thread.
  {         
    struct queue * next;          // Points to the next value in the queue.
    var_t        * value;         // Physical value.
  } * root;                       // Points to the first unread value in the queue.
} thread_t;

//////////////////////
// Global variables.
//////////////////////

extern bool            threading_status ;
extern jmp_buf         station          ;
extern thread_t      * threads          ;
extern unsigned long   tid              ;
extern unsigned long   curr_tid         ;

/////////////////////
// Accessor macros. 
/////////////////////

# define is_threading() (threading_status)
# define jmp_to_station(status) longjmp (station, status)
# define get_next_tid() (tid)
# define get_curr_tid() (curr_tid)
# define get_thread(thread_num) (threads[thread_num])
# define get_curr_thread() (threads[curr_tid])

//////////////////////////////
// Statements and functions.
//////////////////////////////

FACT_INTERN_FUNC (void)   scheduler (void);
FACT_INTERN_FUNC (FACT_t) sprout    (func_t *, word_list);

FACT_EXTERN_BIF (get_tid);

#endif
