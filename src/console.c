#include <stdarg.h>
#include <stdio.h>

#include "console.h"
#include "typedef.h"

int console_init()
{
	return 0;
}

int console_close()
{
	return 0;
}

int console_write(char* format, ...)
{
	va_list arguments;
	va_start(arguments, format);

	printf(format);

	va_end(arguments);

	return 0;
}

int console_yes_or_no()
{
	printf("Yes / No ?");
	return FALSE;
}
