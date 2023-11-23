#ifndef _FILE_H_
#define _FILE_H_

#include <stdio.h>
#include <dirent.h>

typedef struct File File;
typedef struct Directory Directory;
typedef struct DirectoryEntry DirectoryEntry;

struct File
{
	FILE* file;
};

struct Directory
{
	DIR *directory;
};

struct DirectoryEntry
{
	char name[256 + 2];
	int reached_eof;
};

int file_init();

int file_open_for_writing(File*, char*);
int file_open_for_reading(File*, char*);

int file_read(File*, char*, int);
int file_write(File*, char*, int);

int file_close(File*);

char* file_extract_name(char*, int);

int directory_open(Directory*, char*);
int directory_close(Directory*);

int directory_get_entry(Directory*, DirectoryEntry*);

int change_current_directory(char*);
void retrieve_cwd(char *, int);

#endif