#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "ftp.h"
#include "network.h"
#include "typedef.h"
#include "utils.h"

static int terminate = 0;

void new_connection(Socket);

int main(int argc, char* argv[])
{
	Address accepted_address;
	Socket client;
	Socket socket_command;

	network_init();
	accepted_address = network_convert_string_to_address("127.0.0.1");
	if(errno)
	{
		perror("string to address");
		return -1;
	}

	if(network_open(&socket_command, 21))
	{
		perror("command socket");
		return -2;
	}

	while(!terminate)
	{
		network_listen(&socket_command);
		client = network_wait_for_client(&socket_command);
		if(errno)
			break;

#ifndef ACCEPT_ALL_USERS
		if(!network_compare_address(client.address, accepted_address))
		{
			printf("Bad client (Address: %s)\n", network_convert_address_to_string(client.address));
			network_close(&client);
			continue;
		}
#endif

		new_connection(client);
		terminate = 1;	// For debugging purposes
	}

	printf("Stop right there, criminal scum!\n");
	network_close(&client);
	network_close(&socket_command);

	return 0;
}

void new_connection(Socket command_socket)
{
	int read_size;
	Client client;

	client.command = command_socket;
	client.message = (char*) xalloc(COMMAND_BUFFER_SIZE, sizeof(char));

	ftp_new_connection_handler(&client);
	while((client.message_size = network_receive(client.command, client.message, COMMAND_BUFFER_SIZE)) > 0 && !terminate)
	{
		if(ftp_packet_handler(&client) == -1)
			break;
	}
	
	if(read_size == -1)
		perror("recv");

	network_close(&client.data);
	free(client.message);
	printf("The client is disconnected\n");
}
