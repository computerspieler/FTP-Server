#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "ftp.h"
#include "network.h"
#include "typedef.h"

void new_connection(Socket client);

int main(int argc, char* argv[])
{
	Address accepted_address;
	Socket socket_command;
	Socket client;

	network_init();
	accepted_address = network_convert_string_to_address("127.0.0.1");
	if(errno)
	{
		perror("string to address");
		return -1;
	}

	if(network_open(&socket_command, FALSE, 21))
	{
		perror("command socket");
		return -2;
	}

	network_listen(&socket_command);
	while(TRUE)
	{
		client = network_wait_for_client(&socket_command);
		if(errno)
			break;

		if(!network_compare_address(client.address, accepted_address))
		{
			printf("Bad client\n");
			network_close(&client);
			continue;
		}

		new_connection(client);
	}

	network_close(&socket_command);

	return 0;
}

void new_connection(Socket client)
{
	int read_size;
	Socket socket_data;
	char message[COMMAND_BUFFER_SIZE];

	if(network_open(&socket_data, TRUE, 20))
		if(network_open_in_range(&socket_data, TRUE, 1024, 65535))
			return;

	ftp_new_connection_handler(&client);
	while((read_size = network_receive(client, message, COMMAND_BUFFER_SIZE)) > 0)
	{
		if(ftp_packet_handler(&client, message) < 0)
			break;
	}
	
	if(read_size == -1)
		perror("recv");

	network_close(&socket_data);
	printf("The client is disconnected\n");
}
