#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ftp.h"
#include "network.h"

int ftp_new_connection_handler(Socket* client)
{
	printf("New connection\n");
	ftp_send_response(client, 220, "Server", -1);
	return 0;
}

int ftp_packet_handler(Socket* client, char* message)
{
	if(!strncmp(message, "USER", 4))
		ftp_send_response(client, 230, "User logged in", -1);

	else if(!strncmp(message, "SYST", 4))
		ftp_send_response(client, 215, "UNIX Type: L8", -1);

	else if(!strncmp(message, "QUIT", 4))
		return -1;

	else
		printf("New packet:\n%s\n----\n", message);

	return 0;
}

int ftp_send_response(Socket* client, int reply_code, char* string, int string_size)
{
	int return_code;
	char* reply;
	int reply_size;

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

	return_code = network_send(*client, reply, reply_size);
	free(reply);

	return return_code;
}