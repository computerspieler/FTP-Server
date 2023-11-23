#include <errno.h>
#include <stdlib.h>
#include <signal.h>

#include "config.h"
#include "console.h"
#include "file.h"
#include "ftp.h"
#include "network.h"
#include "typedef.h"
#include "utils.h"

static Socket client;
static Socket socket_command;

void new_connection(Socket);
void interrupt_handler(int);
void stopEverything();

int main(int argc, char* argv[])
{
    int i;
    int port;

    port = 21;

#ifndef ACCEPT_ALL_USERS
	Address accepted_address;
	accepted_address = network_convert_string_to_address("127.0.0.1");

	if(errno)
	{
		perror("string to address");
		return -1;
	}
#endif

    for(i = 1; i < argc; i ++)
    {
        if(argv[i][0] != '-')
            continue;

        switch(argv[i][1])
        {
        case 'p':
            i ++;
            port = strtol(argv[i], NULL, 10);
            break;

        default:
            console_write("Invalid argument: %s\n", argv[i]);
            return -1;
        }
    }

	console_init();
	file_init();
	network_init();
	signal(SIGINT, interrupt_handler);

	if(network_open(&socket_command, port))
	{
		perror("command socket");
		return -2;
	}

    console_write("Server listening on port %d\n", port);

	while(1)
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
	}

	stopEverything();

	return 0;
}

void stopEverything()
{
	console_write("Stop right there, criminal scum!\n");
	network_close(&client);
	network_close(&socket_command);
	console_close();
}

void new_connection(Socket command_socket)
{
	Client client;

	clear_memory_area(&client, sizeof(Client));

	client.command = command_socket;
	client.message = (char*) xalloc(COMMAND_BUFFER_SIZE, sizeof(char));

	ftp_new_connection_handler(&client);
	while((client.message_size = network_receive(client.command, client.message, COMMAND_BUFFER_SIZE)) > 0)
	{
		if(ftp_packet_handler(&client) == -1)
			break;
	}

	network_close(&client.data);
	free(client.message);
	console_write("The client is disconnected\n");
}

void interrupt_handler(int dummy)
{
	stopEverything();
	exit(EXIT_SUCCESS);
}
