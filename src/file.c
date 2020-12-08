#include <stddef.h>
#include <stdio.h>

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
	return !(nb_bytes_read == buffer_size);
}

int file_write(File* file, char* buffer, int buffer_size)
{
	size_t nb_bytes_written = fwrite(buffer, sizeof(char), buffer_size, file->file);
	return !(nb_bytes_written == buffer_size);
}

int file_close(File* file)
{
	fclose(file->file);
	return 0;
}