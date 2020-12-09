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
	for(i = 0; argument_str[i] != '\n'; i++)
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
		OPTION_UNIMPLEMENTED;
		break;

	case 'I':   // Binary
		OPTION_UNIMPLEMENTED;
		break;

	default:
		console_write("Unknown type code: %s\n", client->arguments);
		ftp_send_response(client, 500, NULL, -1);
	}
	return 0;
}

HANDLER(MODE)
{
	switch(client->arguments[0])
	{
	case 'S':	// Stream
		OPTION_UNIMPLEMENTED;
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
	return 0;
}
//structure-code
HANDLER(STRU)
{
	switch(client->arguments[0])
	{
	case 'F':	// File
		OPTION_UNIMPLEMENTED;
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
	return 0;
}
//path
HANDLER(RETR)
{
	COMMAND_UNIMPLEMENTED;
	return 0;
}
//path
HANDLER(LIST)
{
	COMMAND_UNIMPLEMENTED;
	return 0;
}

HANDLER(STOR)
{
	File file;
	char* file_name = file_extract_name(client->arguments, client->arguments_size);

	console_write("File to download: %s\n", file_name);
	if(file_open_for_writing(&file, file_name))
	{
		perror("fopen");
		ftp_send_response(client, 451, NULL, -1);
		return 0;
	}

	ftp_send_response(client, 150, NULL, -1);
	if(network_connect(&client->data))
	{
		perror("data socket");
		return -1;
	}
	ftp_send_response(client, 125, NULL, -1);

	clear_buffer(client->message, client->message_size);
	while((client->message_size = network_receive(client->data, client->message, COMMAND_BUFFER_SIZE)) > 0)
	{
		file_write(&file, client->message, client->message_size);
		console_write("%i: %.*s\n", client->message_size, COMMAND_BUFFER_SIZE, client->message);
	}

	if(client->message_size == -1)
		perror("recv");

	network_close(&client->data);
	file_close(&file);

	console_write("Would you like to start this program ?\n");
	if(console_yes_or_no())
		console_write("Start the downloaded program\n");

	return 0;
}

HANDLER(NOOP)
{
	ftp_send_response(client, 200, NULL, -1);
	return 0;
}
