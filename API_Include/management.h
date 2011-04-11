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

#ifdef __cplusplus
extern "C" {
#endif

FACT_API_FUNC (var_t *) alloc_var     ( void );
FACT_API_FUNC (var_t *) get_local_var (func_t *, char *);
FACT_API_FUNC (var_t *) get_var       (func_t *, char *);
FACT_API_FUNC (var_t *) add_var       (func_t *, char *);
FACT_API_FUNC (var_t *) resize_var    (var_t  *, int);
  
FACT_API_FUNC (func_t *) alloc_func     ( void );
FACT_API_FUNC (func_t *) get_local_func (func_t *, char *);
FACT_API_FUNC (func_t *) get_func       (func_t *, char *);
FACT_API_FUNC (func_t *) add_func       (func_t *, char *);
FACT_API_FUNC (func_t *) resize_func    (func_t *, int);
  
FACT_API_DEPRECATED ( void ) scroll (func_t *);

FACT_API_FUNC ( void ) free_var  (var_t  *);
FACT_API_FUNC ( void ) free_func (func_t *);

#ifdef __cplusplus
}
#endif

#endif
