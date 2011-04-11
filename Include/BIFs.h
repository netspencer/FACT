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

#ifndef BIFS_H_
#define BIFS_H_

#include "FACT.h"

////////////
// Macros.
////////////

#define NOARGS ""

//////////////////////
// Inline functions.
//////////////////////

static inline void
FACT_INSTALL_BIF (func_t *scope, BIF dec)
{
  func_t *temp;
  char **get_words (char *);
  
  temp = add_func (scope, dec.name);
  temp->args = get_words (dec.arguments);
  temp->extrn_func = dec.function;
}

/////////////////////
// BIF definitions.
/////////////////////

FACT_EXTERN_BIF (ref);    // Get the address of a variable/function.
FACT_EXTERN_BIF (str);    // Convert a variable to a string array.
FACT_EXTERN_BIF (deref);  // Derefrence an address.
FACT_EXTERN_BIF (print);  // Print a string.
FACT_EXTERN_BIF (sizeof); // Get the size of an array.
FACT_EXTERN_BIF (lock);   // Lock a variable/function.

#endif 
