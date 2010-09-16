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

extern a_type if_statement  (func *, char **, bool *);
extern a_type else_clause  (func *, char **);
extern a_type while_loop (func *, char **);
 
#endif
