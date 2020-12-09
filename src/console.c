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
	char output_char = 0;

	while(output_char != 'y' && output_char != 'n')
	{
		printf("Yes (Y) / No (N) ? ");
		do
			output_char = to_lower(getchar());
		while(output_char < 'a' || output_char > 'z');
	}

	return output_char == 'y';
}
