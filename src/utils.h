#ifndef _UTILS_H_
#define _UTILS_H_

char to_lower(char);

void* xalloc(int, int);
void* xrealloc(void*, int, int);
void data_copy(char*, char*, int);
void clear_buffer(void*, int);

int parse_value(char*, int, int);

int string_length(char*);
void to_lower_string(char*, int);
int string_compare(char*, char*, int);

void print_hex_dump(char*, int);

#endif