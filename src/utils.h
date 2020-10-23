#ifndef _UTILS_H_
#define _UTILS_H_

int absolute(int);
char to_lower(char);

void* xalloc(int, int);
void* xrealloc(void*, int, int);
void data_copy(char*, char*, int);
void clear_buffer(char*, int);

int parse_value(char*, int, int);

int string_length(char*);
void to_lower_string(char*, int);
char* create_substring(char*, int);
int string_compare(char*, char*, int);

#endif