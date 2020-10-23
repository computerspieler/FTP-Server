#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ftp.h"

int ftp_new_connection_handler(Client* client)
{
	printf("New connection\n");
	ftp_send_response(client, 220, "Server", -1);
	return 0;
}

int ftp_packet_handler(Client* client)
{
	int i;
	CommandHandler handler = NULL;

	for(i = 0; i < NB_COMMANDS; i++)
	{
		if(!strncmp(client->message, commands[i].name, commands[i].name_size))
		{
			handler = commands[i].handler;
			break;
		}
	}

	printf("Packet with an unknown command:\n%s\n----\n", client->message);
	if(handler)
	{
		client->arguments = client->message + commands[i].name_size;

		// If there's a space after the command
		// Then we skip it
		if(*client->arguments == ' ')
			client->arguments ++;

		return handler(client);
	}

	ftp_send_response(client, 202, NULL, -1);
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
		string_size = strlen(string);

	reply_size = 5 + string_size + (string_size > 0);
	reply = (char*) calloc(reply_size, sizeof(char));

	if(string_size > 0)
		sprintf(reply, "%d %.*s\r\n", reply_code, string_size, string);
	else
		sprintf(reply, "%d\r\n", reply_code);

	return_code = network_send(client->command, reply, reply_size);
	free(reply);

	return return_code;
}