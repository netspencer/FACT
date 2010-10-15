#ifndef STATEMENTS_H_
#define STATEMENTS_H_

/**
 * Statements.h:
 * Provides functions for if
 * statements and while and for
 * loops.
 *
 * TODO:
 *  - Add for loops.
 *
 * (c) 2010 Matthew Plant - sorry for the bloat.
 */

extern FACT_t invalid_if (func_t *, word_list);
extern FACT_t invalid_else (func_t *, word_list);
extern FACT_t invalid_while (func_t *, word_list);
extern FACT_t invalid_for (func_t *, word_list);

extern FACT_t if_statement  (func_t *, word_list, bool *);
extern FACT_t else_clause  (func_t *, char **);
extern FACT_t while_loop (func_t *, char **);
extern FACT_t for_loop (func_t *, char **);
 
#endif
