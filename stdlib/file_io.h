#ifndef FILE_IO_H_
#define FILE_IO_H_

#include "stdlib.h"

FACT_API_FUNC (FACT_t) run_file_soft (func_t *);
FACT_API_FUNC (FACT_t) run_file_loud (func_t *);
FACT_API_FUNC (FACT_t) open_file     (func_t *);
FACT_API_FUNC (FACT_t) close_file    (func_t *);
FACT_API_FUNC (FACT_t) get_char_file (func_t *);

#endif
