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

#ifndef MANAGEMENT_H_
#define MANAGEMENT_H_

#include "FACT.h"

FACT_INTERN_FUNC (var_t *) alloc_var     (void            );
FACT_INTERN_FUNC (var_t *) get_local_var (func_t *, char *);
FACT_INTERN_FUNC (var_t *) get_var       (func_t *, char *);
FACT_INTERN_FUNC (var_t *) add_var       (func_t *, char *);
FACT_INTERN_FUNC (var_t *) resize_var    (var_t  *, mpz_t );

FACT_INTERN_FUNC (func_t *) alloc_func     (void            );
FACT_INTERN_FUNC (func_t *) get_local_func (func_t *, char *);
FACT_INTERN_FUNC (func_t *) get_func       (func_t *, char *);
FACT_INTERN_FUNC (func_t *) add_func       (func_t *, char *);
FACT_INTERN_FUNC (func_t *) resize_func    (func_t *, mpz_t );

FACT_INTERN_FUNC       (void) free_var  (var_t  *);
FACT_INTERN_FUNC       (void) free_func (func_t *);

#define resize_array      resize_var  
#define resize_func_array resize_func 

#endif
