#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "ftp.h"
#include "network.h"
#include "typedef.h"

Socket m_client;
static Address m_client_ip_address;

static Socket m_socket_command;
static Socket m_socket_data;

int main(int argc, char* argv[])
{
	int read_size;
	char message[COMMAND_BUFFER_SIZE];

	network_init();
	m_client_ip_address = network_convert_string_to_address("127.0.0.1");
	if(errno)
	{
		perror("string to address");
		return -1;
	}

	if(network_open(&m_socket_command, FALSE, 21))
	{
		perror("command socket");
		return -2;
	}

	network_listen(&m_socket_command)
	while(TRUE)
	{
		m_client = network_wait_for_client(&m_socket_command);
		if(errno)
			break;

		if(network_compare_address(m_client.address, m_client_ip_address))
		{
			printf("Bad client\n");
			network_close(&m_client);
			continue;
		}

		if(network_open(&m_socket_data, TRUE, 20))
			if(network_open_in_range(&m_socket_data, TRUE, 1024, 65535))
				break;

		ftp_new_connection_handler();
		while((read_size = network_receive(m_client, message, COMMAND_BUFFER_SIZE)) > 0)
		{
			if(ftp_packet_handler(message) < 0)
				break;
		}
		
		if(read_size == -1)
			perror("recv");

		network_close(&m_socket_data);
		printf("The client is disconnected\n");
	}

	network_close(&m_socket_command);

	return 0;
}
