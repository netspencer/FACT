#ifndef MATH_H_
#define MATH_H_

/**
 * math:
 * provides various math operations, such as
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
extern a_type num_to_var (char *);      /* converts a string to an amigious
					   type.
					*/

extern a_type add (a_type, a_type);    /* adds two numbers together */
extern a_type sub (a_type, a_type);    /* subtracts two numbers */
extern a_type mult (a_type, a_type);   /* multiplies two numbers */
extern a_type divide (a_type, a_type); /* divides two numbers */
extern a_type mod (a_type, a_type);    /* modulos two numbers */

extern a_type paren (func *, char **); /* isolates all the characters between
					  two parenthesis and then evaluates
					  them.
				       */

#endif
