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
FACT_INTERN_FUNC (syn_tree_t) make_syn_tree (char **, int *);
			     
/* Evaluate and expression or single instruction, or even
 * an entire block of code (procedure/lambda_proc).
 */
FACT_INTERN_FUNC (FACT_t) eval_expression (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) eval            (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) procedure       (func_t *, syn_tree_t);
FACT_INTERN_FUNC (FACT_t) lambda_proc     (func_t *, syn_tree_t);

#endif
