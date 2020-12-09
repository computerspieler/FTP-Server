#include <stdio.h>
#include <stdlib.h>

#include "console.h"
#include "ftp.h"
#include "utils.h"
#include "typedef.h"

int ftp_new_connection_handler(Client* client)
{
	console_write("New connection\n");
	ftp_send_response(client, 220, "Server", -1);
	return 0;
}

int ftp_packet_handler(Client* client)
{
	int i;
	Command* command = NULL;

	// It will look for a <CRLF> sequence and turn it into a null character
	for(i = client->message_size; i > 1; i--)
	{
		if(client->message[i] == '\n' && client->message[i-1] == '\r')
		{
			client->message[i]   = 0;
			client->message[i-1] = 0;
			client->message_size = i-1;
			break;
		}
	}

	for(i = 0; i < NB_COMMANDS; i++)
	{
		if(string_compare(client->message, commands[i].name, commands[i].name_size))
		{
			command = &commands[i];
			break;
		}
	}

	console_write("Packet:\n");
	print_hex_dump(client->message, client->message_size);
	console_write("----\n");
	
	console_write(commands[i].name);

	if(command)
	{
		client->arguments = client->message + command->name_size;
		client->arguments_size = client->message_size - command->name_size;

		// If there's a space after the command
		// Then we skip it
		if(*client->arguments == ' ')
			client->arguments ++;

		return command->handler(client);
	}

	ftp_send_response(client, 500, NULL, -1);
	return 0;
}

int ftp_send_response(Client* client, int reply_code, char* string, int string_size)
{
	char* reply;
	int reply_size;
	int return_code;

	if(string == NULL)
		string_size = 0;

	if(string_size < 0)
		string_size = string_length(string);

	reply_size = 5 + string_size + (string_size > 0);
	reply = (char*) xalloc(reply_size, sizeof(char));

	if(string_size > 0)
		sprintf(reply, "%d %.*s\r\n", reply_code, string_size, string);
	else
		sprintf(reply, "%d\r\n", reply_code);

	return_code = network_send(client->command, reply, reply_size);
	free(reply);

	return return_code;
}