#include <dirent.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "file.h"

char* file_extract_name(char* path, int path_length)
{
	int i;
	char* file_name = NULL;

	for(i = path_length; i > 0; i --)
	{
		if(path[i-1] == '/')
		{
			file_name = &path[i];
			break;
		}
	}

	if(file_name == NULL)
		file_name = path;

	return file_name;
}

int file_init()
{
	return 0;
}

int file_open_for_writing(File* file, char* name)
{
	file->file = fopen(name, "w");
	return file->file == NULL;
}

int file_open_for_reading(File* file, char* name)
{
	file->file = fopen(name, "r");
	return file->file == NULL;
}

int file_read(File* file, char* buffer, int buffer_size)
{
	size_t nb_bytes_read = fread(buffer, sizeof(char), buffer_size, file->file);
	return !((int) nb_bytes_read == buffer_size);
}

int file_write(File* file, char* buffer, int buffer_size)
{
	size_t nb_bytes_written = fwrite(buffer, sizeof(char), buffer_size, file->file);
	return !((int) nb_bytes_written == buffer_size);
}

int file_close(File* file)
{
	fclose(file->file);
	return 0;
}

int directory_open(Directory* d, char* path)
{
	d->directory = opendir(path);
	if(!d->directory)
	{
		perror("opendir");
		return -1;
	}

	return 0;
}

int directory_close(Directory* d)
{
	if(d->directory)
		closedir(d->directory);

	return 0;
}

int directory_get_entry(Directory* d, DirectoryEntry* entry)
{
	struct dirent* ent;

	ent = readdir(d->directory);

	if(!ent)
	{
		if(errno == EBADF)
		{
			perror("readdir");
			return -1;
		}

		if(entry)
			entry->reached_eof = 1;
	}
	else if(entry)
	{
		strncpy(entry->name, ent->d_name, 256);
		entry->reached_eof = 0;
	}

	return 0;
}

int change_current_directory(char* path)
{
	return chdir(path);
}

void retrieve_cwd(char * buffer, int size)
{
	getcwd(buffer, size-1);
	buffer[size-1] = 0;
}