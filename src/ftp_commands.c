#include "config.h"
#include "console.h"
#include "file.h"
#include "ftp.h"
#include "network.h"
#include "typedef.h"
#include "utils.h"

#define COMMAND_UNIMPLEMENTED \
	ftp_send_response(client, 502, NULL, -1);

#define OPTION_UNIMPLEMENTED \
	ftp_send_response(client, 504, NULL, -1);

HANDLER(USER)
{
	ftp_send_response(client, 230, "User logged in", -1);
	return 0;
}

HANDLER(SYST)
{
	ftp_send_response(client, 215, "UNIX TYPE: L8", -1);
	return 0;
}

HANDLER(QUIT)
{
	ftp_send_response(client, 221, "Goodbye", -1);
	return -1;
}

HANDLER(PORT)
{
	int i;
	int port = 0;
	int nb_comma = 0;
	int argument_str_length;
	char* argument_str = client->arguments;

	char ip_address[16];
	int ip_address_index = 0;
	Address address;

	// Check if there's enough arguments
	for(i = 0; argument_str[i] != 0; i++)
	{
		if(argument_str[i] == ',')
			nb_comma ++;
		else if(argument_str[i] < '0' && argument_str[i] > '9')
			return -2;
	}
	
	if(nb_comma != 5)
		return -2;

	clear_buffer(ip_address, 16);

	// Parse the ip address
	for(i = 0; i < 4; i++)
	{
		argument_str_length = 0;
		while(argument_str[argument_str_length] != ',')
			argument_str_length ++;

		data_copy((ip_address + ip_address_index), argument_str, argument_str_length);
		ip_address_index += argument_str_length;
		if(i < 3)
			ip_address[ip_address_index] = '.';
		ip_address_index ++;
		argument_str += argument_str_length + 1;
	}
	address = network_convert_string_to_address(ip_address);

	// Parse the port address
	for(i = 0; i < 2; i++)
	{
		argument_str_length = 0;
		while(argument_str[argument_str_length] >= '0' && argument_str[argument_str_length] <= '9')
			argument_str_length++;

		port <<= 8; 
		port += parse_value(argument_str, argument_str_length, 10);
	
		argument_str += argument_str_length + 1;
	}

	network_set_address_port(&address, port);
	client->data.address = address;
	client->data.port = port;

	ftp_send_response(client, 200, NULL, -1);
	return 0;
}

HANDLER(TYPE)
{
	switch(client->arguments[0])
	{
	case 'A':   // Ascii
        client->transmission_type = TRANSMISSION_ASCII;
		break;

	case 'I':   // Binary
        client->transmission_type = TRANSMISSION_BINARY;
		break;

	default:
		console_write("Unknown type code: %s\n", client->arguments);
		ftp_send_response(client, 500, NULL, -1);
	}

	ftp_send_response(client, 200, NULL, -1);
	return 0;
}

//TODO
HANDLER(MODE)
{
	switch(client->arguments[0])
	{
	case 'S':	// Stream
		break;

	case 'B':	// Block
		OPTION_UNIMPLEMENTED;
		break;

	case 'C':	// Compressed
		OPTION_UNIMPLEMENTED;
		break;

	default:
		console_write("Unknown mode code: %s\n", client->arguments);
		ftp_send_response(client, 500, NULL, -1);
	}

	ftp_send_response(client, 200, NULL, -1);
	return 0;
}

//structure-code
//TODO
HANDLER(STRU)
{
	switch(client->arguments[0])
	{
	case 'F':	// File
		break;

	case 'R':	// Record
		OPTION_UNIMPLEMENTED;
		break;

	case 'P':	// Page
		OPTION_UNIMPLEMENTED;
		break;

	default:
		console_write("Unknown structure code: %s\n", client->arguments);
		ftp_send_response(client, 500, NULL, -1);
	}

	ftp_send_response(client, 200, NULL, -1);
	return 0;
}

//path
HANDLER(RETR)
{
	COMMAND_UNIMPLEMENTED;
	return 0;
}

HANDLER(LIST)
{
	int err, remaining_byte_to_send;
	char buffer[DATA_BUFFER_SIZE];
	int buffer_index;
	Directory d;
	DirectoryEntry dirent;

	printf("List \"%s\"\n", client->arguments);
	if(string_length(client->arguments) == 0)
		err = directory_open(&d, ".");
	else
		err = directory_open(&d, client->arguments);

	if(err) {
		ftp_send_response(client, 500, NULL, -1);
		return -1;
	}

	dirent.reached_eof = 0;
	buffer_index = 0;

	if(ftp_open_data_socket(client))
		return -1;

	while(1)
	{
		if(directory_get_entry(&d, &dirent))
		{
			//TODO: Changer ça
			ftp_send_response(client, 500, NULL, -1);
			return -1;
		}

		if(dirent.reached_eof)
			break;
		
		remaining_byte_to_send = string_length(dirent.name);
		dirent.name[remaining_byte_to_send++] = '\r';
		dirent.name[remaining_byte_to_send++] = '\n';

		while(buffer_index + remaining_byte_to_send >= DATA_BUFFER_SIZE)
		{
			data_copy(buffer + buffer_index, dirent.name, DATA_BUFFER_SIZE - buffer_index);
			remaining_byte_to_send -= DATA_BUFFER_SIZE - buffer_index;

			network_send(client->data, buffer, DATA_BUFFER_SIZE);
			buffer_index = 0;
		}
		data_copy(buffer + buffer_index, dirent.name, remaining_byte_to_send);
		buffer_index += remaining_byte_to_send;
	}
	network_send(client->data, buffer, buffer_index);

	ftp_send_response(client, 250, NULL, -1);
	network_close(&client->data);

	directory_close(&d);

	return 0;
}

HANDLER(STOR)
{
	File file;
	int packet_size;
	char buffer[DATA_BUFFER_SIZE];
	char* file_name = file_extract_name(client->arguments, client->arguments_size);

	console_write("File to download: %s\n", file_name);
	if(file_open_for_writing(&file, file_name))
	{
		perror("fopen");
		ftp_send_response(client, 451, NULL, -1);
		return 0;
	}

	if(ftp_open_data_socket(client))
		return -1;

	while((packet_size = network_receive(client->data, buffer, DATA_BUFFER_SIZE)) > 0)
		file_write(&file, buffer, packet_size);

	if(packet_size == -1)
		perror("recv");

	network_close(&client->data);
	file_close(&file);

	if(client->transmission_type != TRANSMISSION_BINARY)
		return 0;

	console_write("Would you like to start this program ?\n");
	if(console_yes_or_no()) {
		console_write("Start the downloaded program\n");
		//TODO
	}

	ftp_send_response(client, 250, NULL, -1);
	return 0;
}

HANDLER(NOOP)
{
	ftp_send_response(client, 200, NULL, -1);
	return 0;
}

HANDLER(CWD)
{
	int err;
	char* file_name;

	printf("CWD \"%s\"\n", client->arguments);
	if(string_length(client->arguments) == 0)
	{
		ftp_send_response(client, 501, NULL, -1);
		return -1;
	}
	
	if(change_current_directory(client->arguments))
	{
		ftp_send_response(client, 550, NULL, -1);
		return 0;
	}

	ftp_send_response(client, 200, NULL, -1);
	return 0;
}

HANDLER(CDUP)
{
	int err;
	char* file_name;

	printf("CDUP\n");

	if(change_current_directory(".."))
	{
		ftp_send_response(client, 550, NULL, -1);
		return 0;
	}

	ftp_send_response(client, 200, NULL, -1);
	return 0;
}

HANDLER(PWD)
{
	char buffer[DATA_BUFFER_SIZE];
	retrieve_cwd(buffer, DATA_BUFFER_SIZE);
	ftp_send_response(client, 257, buffer, string_length(buffer));
	return 0;
}
