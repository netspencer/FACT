#ifndef BYTECODE_H_
#define BYTECODE_H_

#include "FACT.h"

/* FACT Bytcode simple guide:
 * Scripts are translated into bytecode once after execution. Bytecode compiled code
 * works within the same word_list structure as non-compiled code. The first byte
 * in an instruction begins with a 1, to indicate that it is in fact an instruction.
 * The next byte indicates the category of the instruction. This is a complete
 * list of instruction categories:
 * STOP      (0x0),
 * IGNORE    (0x1),
 * STATEMENT (0x2),
 * MATH_CALL (0x3),
 * PRIMITIVE (0x4),
 * NUMBER    (0x5),
 * The contents of the byte that follows is dependent on the category. MATH_CALL
 * and PRIMITIVE is followed by a byte representing the number of the instruction.
 * NUMBERs, however, are followed by four to eight bytes (depending on the system)
 * that is cast into a var_t pointer, which points to a pre-allocated number. 
 */

#define BYTECODE 0x1

typedef enum bytecode_type 
  {
    STOP      = 0x0, // Signifies the end of an expression.
    IGNORE    = 0x1, // Ignore this byte.
    STATEMENT = 0x2, // This includes keywords such as if, else, and return.
    MATH_CALL = 0x3, // +, -, *, etc. 
    PRIMITIVE = 0x4, // Some special things.
    NUMBER    = 0x5, // Numbers are pre-compiled.
  } bytecode_type;

/* This is a bunch of three letter representations of the FACT
 * bytecode instruction set.
 */
enum instruction_set
  {
    BRK = 0x0, // Break.
    ELS = 0x1, // Else.
    FRL = 0x2, // For loop.
    IFS = 0x3, // If statement.
    ONE = 0x4, // On_error.
    RTN = 0x5, // Return.
    SPT = 0x6, // Sprout.
    WHL = 0x7, // While loop.
  };

#define byte char

FACT_INLINE_FUNC (char *) get_bcode_label (byte *);

FACT_INTERN_FUNC (void) compile_to_bytecode (char **);
FACT_INTERN_FUNC (void) compile_constants   (char **);

#endif
