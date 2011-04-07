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
