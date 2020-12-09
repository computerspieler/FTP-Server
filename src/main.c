#include <errno.h>
#include <stdlib.h>

#include "config.h"
#include "console.h"
#include "file.h"
#include "ftp.h"
#include "network.h"
#include "typedef.h"
#include "utils.h"

static int terminate = 0;

void new_connection(Socket);

int main(int argc, char* argv[])
{
	Socket client;
	Socket socket_command;

#ifndef ACCEPT_ALL_USERS
	Address accepted_address;
	accepted_address = network_convert_string_to_address("127.0.0.1");

	if(errno)
	{
		perror("string to address");
		return -1;
	}
#endif

	(void) argc;
	(void) argv;

	console_init();
	file_init();
	network_init();

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
			console_write("Bad client (Address: %s)\n", network_convert_address_to_string(client.address));
			network_close(&client);
			continue;
		}
#endif

		new_connection(client);
		terminate = 1;	// For debugging purposes
	}

	console_write("Stop right there, criminal scum!\n");
	network_close(&client);
	network_close(&socket_command);
	console_close();

	return 0;
}

void new_connection(Socket command_socket)
{
	Client client;

	client.command = command_socket;
	client.message = (char*) xalloc(COMMAND_BUFFER_SIZE, sizeof(char));

	ftp_new_connection_handler(&client);
	while((client.message_size = network_receive(client.command, client.message, COMMAND_BUFFER_SIZE)) > 0 && !terminate)
	{
		if(ftp_packet_handler(&client) == -1)
			break;
	}

	network_close(&client.data);
	free(client.message);
	console_write("The client is disconnected\n");
}
