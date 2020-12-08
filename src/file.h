#ifndef _FILE_H_
#define _FILE_H_

#include <stdio.h>

typedef struct File File;

struct File
{
	FILE* file;
	char* filename;
};

int file_init();

int file_open_for_writing(File*, char*);
int file_open_for_reading(File*, char*);

int file_read(File*, char*, int);
int file_write(File*, char*, int);

int file_close(File*);

char* file_extract_name(char*, int);

#endif