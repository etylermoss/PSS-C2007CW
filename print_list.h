#include <stddef.h>
#include <stdio.h>

struct ListOptions
{
	size_t items;
	char* associator;
	char* delimiter;
};

enum ListItemType
{
	VAL_INT,
	VAL_UINT,
	VAL_LONG,
	VAL_ULONG,
	VAL_FLOAT,
	VAL_DOUBLE,
	VAL_CHAR,
	VAL_STR,
	VAL_PTR,
};

struct ListItem
{
	const char* key;
	enum ListItemType value_type; /* must match type of value */
	const void* value;
};

void printf_list(const struct ListOptions options, ...);
void fprintf_list(FILE* file, const struct ListOptions options, ...);
void aprintf_list(const struct ListOptions options, struct ListItem* items);
void afprintf_list(FILE* file, const struct ListOptions options, struct ListItem* items);