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

#include "FACT.h"

FACT_t
FACT_throw_error (func_t *scope, const char *desc, syn_tree_t exp)
{
  char **i;
  FACT_t ret_val =
    {
      .type = ERROR_TYPE,
      .error =
      {
        .line = scope->line,
        .description = (char *) desc,
        .scope = scope,
      },
    };

  if (exp.lines != NULL)
    {
      for (i = exp.base; i <= exp.syntax; i++)
        ret_val.error.line += *(exp.lines++);
    }
  
  return ret_val;
}

void
errorman_dump (error_t error)
{
  fprintf (stderr, "E> Caught error \"%s\" from %s at (%s:%d).\n", error.description, error.scope->name, error.scope->file_name, error.line);
}
