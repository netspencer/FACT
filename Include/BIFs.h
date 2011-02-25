#ifndef BIFS_H_
#define BIFS_H_

#include "FACT.h"

//////////////////////
// Inline functions.
//////////////////////

static inline void
FACT_INSTALL_BIF (func_t *scope, char *name, BIF dec)
{
  func_t * temp;
  char ** get_words (char *);
  
  temp = add_func (scope, name);
  temp->args = get_words (dec.arguments);
  temp->extrn_func = dec.function;
}

/////////////////////
// BIF definitions.
/////////////////////

FACT_EXTERN_BIF (ref   ); // Get the address of a variable/function.
FACT_EXTERN_BIF (deref ); // Derefrence an address.
FACT_EXTERN_BIF (print ); // Print a string.
FACT_EXTERN_BIF (sizeof); // Get the size of an array.

#endif 