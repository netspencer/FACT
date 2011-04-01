#ifndef FACT_H_
#define FACT_H_

#define FACT_API_FUNC(type) type
#if (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
# define FACT_API_DEPRECATED(type) __attribute__((__deprecated__)) type
#else
# define FACT_API_DEPRECATED(type) type
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <gmp.h>

#include <FACT/typedefs.h>
#include <FACT/mpc_functions.h>
#include <FACT/utilities.h>
#include <FACT/malloc_replacements.h>
#include <FACT/management.h>
#include <FACT/execfile.h>
/* more to come */

static inline FACT_t
FACT_noline_throw (func_t *scope, const char *desc)
{
  FACT_t ret_val =
    {
      .type = ERROR_TYPE,
      .error =
      {
        .line = 0,
        .scope = scope,
        .description = (char *) desc,
      },
    };
  return ret_val;
}

#define FACT_ret_error return FACT_noline_throw

#endif
