#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ftp.h"
#include "globals.h"
#include "network.h"
#include "typedef.h"

Client m_client;
static struct sockaddr_in m_client_ip_address;

static int m_socket_command;
static int m_socket_data;

static struct sockaddr_in m_socket_command_address;
static struct sockaddr_in m_socket_data_address;

int network_init(const char* client_address)
{
	if(!inet_aton(client_address, &m_client_ip_address.sin_addr))
	{
		perror("inet_aton");
		return -1;
	}

	m_socket_command = socket_open(&m_socket_command_address, 21, FALSE);
	perror("Command socket");

	return (m_socket_command < 0);
}

int socket_open(struct sockaddr_in *server_address, int port, int set_to_non_blocking)
{
	int flags;
	int socket_out;

	socket_out = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_out == -1)
		return -1;

	server_address->sin_family = AF_INET;
	server_address->sin_addr.s_addr = INADDR_ANY;
	server_address->sin_port = htons(port);
	if(bind(socket_out, (struct sockaddr*) server_address, sizeof(struct sockaddr_in)) < 0)
	{
		close(socket_out);
		return -2;
	}

	if(!set_to_non_blocking)
		return socket_out;

	flags = fcntl(socket_out, F_GETFL, 0);
	if(flags == -1)
	{
		close(socket_out);
		return -3;
	}

	if(fcntl(socket_out, F_SETFL, flags | O_NONBLOCK))
	{
		close(socket_out);
		return -4;
	}

	return socket_out;
}

void network_run()
{
	Client client;

	listen(m_socket_command, 3);
	while(TRUE)
	{
		printf("Waiting for clients\n");
		client.address_size = sizeof(struct sockaddr_in);
		client.socket = accept(m_socket_command, (struct sockaddr*) &client.address, (socklen_t*) &client.address_size);
		if(client.socket < 0)
		{
			perror("accept");
			return;
		}

		if(client.address.sin_addr.s_addr != m_client_ip_address.sin_addr.s_addr)
		{
			printf("Wrong client: %s\n", inet_ntoa(m_client_ip_address.sin_addr));
			close(client.socket);
			continue;
		}

		network_new_connection(client);
		printf("The client disconnected\n");
	}
}

void network_exit()
{
	close(m_socket_command);
}

int network_send(Client client, char* buffer, int buffer_size)
{
	return send(client.socket, buffer, buffer_size, 0);
}

int network_receive(Client client, char* buffer, int buffer_size)
{
	return recv(client.socket, buffer, buffer_size, 0);
}

void network_new_connection(Client client)
{
	int port;
	int read_size;
	int return_code;
	char message[COMMAND_BUFFER_SIZE];

	m_client = client;
	m_socket_data = 0;

	// It will look for an available port
	for(port = 1024; m_socket_data == 0; port ++)
	{
		if(port >= 65635)
		{
			printf("No available port between 1024 and 65635\n");
			return;
		}

		m_socket_data = socket_open(&m_socket_data_address, 20, TRUE);
		
		if(m_socket_data == -2 && errno == EADDRINUSE)
			continue;

		if(m_socket_data > 0 && errno == 0)
			break;

		perror("Data socket");
		return;
	}

	ftp_new_connection_handler();
	while((read_size = network_receive(client, message, COMMAND_BUFFER_SIZE)) > 0)
	{
		return_code = ftp_packet_handler(message);
		if(return_code < 0)
			break;
	}
	
	if(read_size == -1)
		perror("recv");

	close(m_socket_data);
}
