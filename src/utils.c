#include <stdlib.h>

#include "console.h"
#include "utils.h"
#include "typedef.h"

char to_lower(char c)
{
	return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c;
}

void to_lower_string(char* text, int text_size)
{
	int i;

	for(i = 0; i < text_size; i++)
		text[i] = to_lower(text[i]);
}

int parse_value(char* string, int string_size, int base)
{
	int i;
	int output = 0;
	int converted_digit;

	to_lower_string(string, string_size);

	for(i = 0; i < string_size; i++)
	{
		output *= base;

		if(string[i] >= '0' && string[i] <= '9')
			converted_digit = string[i] - '0';
		else if(string[i] >= 'a' && string[i] <= 'z')
			converted_digit = string[i] - 'a' + 10;
		else
			return 0;
		
		if(converted_digit < base)
			output += converted_digit;
		else
			return 0;
	}

	return output;
}

int string_compare(char* str_a, char* str_b, int size)
{
	int i;

	if(size >= 0)
	{
		for(i = 0; i < size; i++)
			if(str_a[i] != str_b[i])
				return 0;
	}
	else
	{
		i = 0;
		while(str_a[i] != 0 && str_b[i] != 0)
		{
			if(str_a[i] != str_b[i])
				return 0;
			i++;
		}
	}
	
	return 1;
}

void data_copy(char* destination, char* source, int size)
{
	int i;
	if(!source || !destination)
		return;

	for(i = 0; i < size; i++)
		destination[i] = source[i];
}

int string_length(char* text)
{
	int i;

    if(!text)
        return 0;

	for(i = 0; text[i] != 0; i++);

	return i;
}

void* xalloc(int nb, int size)
{
	int i;
	char* pointer = calloc(nb, size);

	if(pointer == NULL)
		return (void*) pointer;

	for(i = 0; i < nb * size; i++)
		pointer[i] = 0;

	return (void*) pointer;
}

void* xrealloc(void* original_ptr, int nb, int size)
{
	void* pointer = realloc(original_ptr, nb * size);

	return pointer;
}

int absolute(int i)
{
	return i < 0 ? -i : i;
}

char* create_substring(char* source, int size)
{
	char* output = (char*) xalloc(size + 1, sizeof(char));
	data_copy(output, source, size);

	return output;
}

void clear_buffer(char* buffer, int size)
{
	int i;
	for(i = 0; i < size; i++)
		buffer[i] = 0;
}

void print_hex_dump(char* string, int string_size)
{
	int i;
	int j;
	int line_length = 16;

	for(i = 0; i < string_size; i += 16)
	{
		for(j = 0; j < line_length; j++)
		{
			if(i + j < string_size)
				console_write("%02x ", (uchar) string[i+j]);
			else
				console_write("   ");
		}

		for(j = 0; j < line_length; j++)
		{
			switch(string[i+j])
			{
			case '\r': console_write("\\r"); break;
			case '\n': console_write("\\n"); break;
			case '\t': console_write("\\t"); break;

			default: console_write("%c", string[i+j]);
			}
		}
		console_write("\n");
	}
}

void clear_memory_area(void* buf, int n) 
{
	while(n >= 0) {
		n --;
		((char*)buf)[n] = 0;
	}
}