#ifndef BIFS_H_
#define BIFS_H_

#include "FACT.h"

/* BIF definitions. */

FACT_EXTERN_BIF (ref   ); // Get the address of a variable/function.
FACT_EXTERN_BIF (deref ); // Derefrence an address.
FACT_EXTERN_BIF (sizeof); // Get the size of an array.

#endif 
