#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "util.h"

void fatal_error(const char* str)
{
	fprintf(stderr, "FATAL ERROR: %s\n", str);

	exit(1);
}

void fatal_errorf(const char* fmt, ...)
{
	fputs("FATAL ERROR: ", stderr);

	va_list argp;
	va_start(argp, fmt);

	vfprintf(stderr, fmt, argp);

	va_end(argp);

	fputc('\n', stderr);

	exit(1);
}