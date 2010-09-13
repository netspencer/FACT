#ifndef _SHELL_H
#define _SHELL_H

#include "common.h"

extern char *get_input (const char *line_begin, const char *incomplete, FILE *fp);
extern void shell (void);

#endif
