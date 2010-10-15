#ifndef MATH_H_
#define MATH_H_

/**
 * math:
 * provides var_tious math operations, such as
 * addition, subtraction, etc.
 *
 * Linked from Math.c
 *
 * TODO:
 * - Add support for bitwise operations.
 *
 * (c) 2010 Matthew Plant. Sorry for the bloat.
 */

extern bool isnum (char *);             /* returns true if a string of characters
					   is a valid base 10 number, and returns
					   false otherwise.
					*/
extern FACT_t num_to_var (char *);      /* converts a string to an amigious
					   type.
					*/

extern FACT_t add (FACT_t, FACT_t);    /* adds two numbers together */
extern FACT_t sub (FACT_t, FACT_t);    /* subtracts two numbers */
extern FACT_t mult (FACT_t, FACT_t);   /* multiplies two numbers */
extern FACT_t divide (FACT_t, FACT_t); /* divides two numbers */
extern FACT_t mod (FACT_t, FACT_t);    /* modulos two numbers */

extern FACT_t paren (func_t *, word_list); /*
					   isolates all the characters between
					   two parenthesis and then evaluates
					   them.
					 */
extern FACT_t add_assignment (FACT_t, FACT_t);
extern FACT_t sub_assignment (FACT_t, FACT_t);
extern FACT_t mult_assignment (FACT_t, FACT_t);
extern FACT_t div_assignment (FACT_t, FACT_t);
extern FACT_t mod_assignment (FACT_t, FACT_t);

#endif
