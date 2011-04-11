/* This file is part of FACT.
 *
 * FACT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FACT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FACT. If not, see <http://www.gnu.org/licenses/>.
 */

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
 * CONSTANT  (0x5),
 * The contents of the byte that follows is dependent on the category. MATH_CALL
 * and PRIMITIVE is followed by a byte representing the number of the instruction.
 * CONSTANTs, however, are followed by four to eight bytes (depending on the
 * system) that is cast into a var_t pointer, which points to a pre-allocated
 * value.
 */

#define BYTECODE 0x1

typedef enum bytecode_type 
  {
    STOP      = 0x0, // Signifies the end of an expression.
    IGNORE    = 0x1, // Ignore this byte.
    STATEMENT = 0x2, // This includes keywords such as if, else, and return.
    MATH_CALL = 0x3, // +, -, *, etc. 
    PRIMITIVE = 0x4, // Some special things.
    CONSTANT  = 0x5, // Constants are pre-compiled.
  } bytecode_type;

/* This is a bunch of three letter representations of the FACT
 * bytecode instruction set.
 */
enum instruction_set
  {
    BRK = 0x0, // Break.
    ELS = 0x1, // Else.
    ENN = 0x2, // Error.
    FRL = 0x3, // For loop.
    GIV = 0x4, // Give
    IFS = 0x5, // If statement.
    RTN = 0x6, // Return.
    WHL = 0x7, // While loop.
  };

#define byte char

FACT_INLINE_FUNC (char *) get_bcode_label (byte *);

FACT_INTERN_FUNC (void) compile_to_bytecode (char **);
FACT_INTERN_FUNC (void) compile_constants   (char **);

#endif
