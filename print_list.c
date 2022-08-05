#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "print_list.h"

struct ListOptions parse_options(const struct ListOptions options)
{
	struct ListOptions new_options = {
		.items = options.items,
		.associator = " = ",
		.delimiter = ", ",
	};

	if (options.associator && *options.associator != '\0')
		new_options.associator = options.associator;
	if (options.delimiter && *options.delimiter != '\0')
		new_options.delimiter = options.delimiter;

	return new_options;
}

void fprint_item(FILE* file, const struct ListOptions options, const size_t items_left, const struct ListItem item)
{
	fputs(item.key, file);
	fputs(options.associator, file);

	switch (item.value_type) {
	case VAL_INT:    fprintf(file, "%d", *(int*) item.value); break;
	case VAL_UINT:   fprintf(file, "%i", *(unsigned int*) item.value); break;
	case VAL_LONG:   fprintf(file, "%ld", *(long int*) item.value); break;
	case VAL_ULONG:  fprintf(file, "%lu", *(unsigned long int*) item.value); break;
	case VAL_FLOAT:  fprintf(file, "%f", *(float*) item.value); break;
	case VAL_DOUBLE: fprintf(file, "%lf", *(double*) item.value); break;
	case VAL_CHAR:   putc(*(char*) item.value, file); break;
	case VAL_STR:    fputs((char*) item.value, file); break;
	case VAL_PTR:    fprintf(file, "%p", item.value); break;
	default:         fputs("UNKNOWN_TYPE", file);
	}

	if (items_left > 1)
		fputs(options.delimiter, file);
};

void vfprintf_list(FILE* file, struct ListOptions options, va_list argp)
{
	size_t items_left = options.items;

	while (items_left != 0) {
		struct ListItem item = va_arg(argp, struct ListItem);

		fprint_item(file, options, items_left, item);

		items_left--;
	}
};

void printf_list(struct ListOptions options, ...)
{
	va_list argp;
	va_start(argp, options);

	options = parse_options(options);

	vfprintf_list(stdout, options, argp);

	va_end(argp);
};

void fprintf_list(FILE* file, struct ListOptions options, ...)
{
	va_list argp;
	va_start(argp, options);

	options = parse_options(options);

	vfprintf_list(file, options, argp);

	va_end(argp);
};

void aprintf_list(struct ListOptions options, struct ListItem* items)
{
	afprintf_list(stdout, options, items);
};

void afprintf_list(FILE* file, struct ListOptions options, struct ListItem* items)
{
	options = parse_options(options);

	for (size_t items_left = options.items; items_left != 0; items_left--)
		fprint_item(file, options, items_left, items[options.items - items_left]);
};