#ifndef SHELL_H_
#define SHELL_H_

#include "common.h"

extern int line_number;

extern char *get_input (FILE *);
extern void shell (func *);

#endif
