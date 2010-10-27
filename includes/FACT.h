#ifndef FACT_H_
#define FACT_H_

#define FACT_API_FUNC(type) extern type
#if (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#define FACT_API_DEPRECATED(type) extern __attribute__((__deprecated__)) type
#else
#define FACT_API_DEPRECATED(type) extern type
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <gmp.h>

#include "typedefs.h"
#include "mpc_functions.h"
#include "utilities.h"
#include "malloc_replacements.h"
#include "management.h"
/* more to come */

#endif
