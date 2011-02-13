#ifndef EVAL_H_
#define EVAL_H_

#include "FACT.h"

/* Get or set the instruction pointer. */
FACT_INLINE_FUNC (unsigned long) get_ip    ( void );
FACT_INLINE_FUNC ( void )        set_ip    (unsigned long);
FACT_INLINE_FUNC ( void )        move_ip   (unsigned long);
FACT_INLINE_FUNC ( void )        next_inst ( void );

/* This macro is used to make reseting the ip
 * easier and more logical in certain contexts.
 */
#define reset_ip() set_ip (0)

/* Convert a char ** to a word_list. */
FACT_INTERN_FUNC (word_list) make_word_list (char **, bool);
			     
/* Evaluate and expression or single instruction, or even
 * an entire block of code (procedure/lambda_proc).
 */
FACT_INTERN_FUNC (FACT_t) eval_expression (func_t *, word_list);
FACT_INTERN_FUNC (FACT_t) eval            (func_t *, word_list);
FACT_INTERN_FUNC (FACT_t) procedure       (func_t *, word_list);
FACT_INTERN_FUNC (FACT_t) lambda_proc     (func_t *, word_list);

#endif
