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

#ifndef PARSER_H_
#define PARSER_H_

#include "FACT.h"

FACT_INTERN_FUNC (int) op_get_prec (char *);
FACT_INTERN_FUNC (int) get_exp_length (char **, int);
FACT_INTERN_FUNC (int) get_exp_length_first (char **, int);

FACT_INTERN_FUNC (int *) get_newlines (char **);

FACT_INTERN_FUNC (char **) get_words (char *);
FACT_INTERN_FUNC (char **) parse (char **, const char *, int, int **);

#endif
