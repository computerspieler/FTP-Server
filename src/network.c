#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "network.h"
#include "typedef.h"

#define MAX_MESSAGE_SIZE 8

static struct sockaddr_in client_ip_address;

static int m_socket_data;
struct sockaddr_in m_socket_data_address;

static int m_socket_command;
struct sockaddr_in m_socket_command_address;

int network_init(const char* client_address)
{
	if(!inet_aton(client_address, &client_ip_address.sin_addr))
	{
		perror("inet_aton");
		return -1;
	}

	m_socket_data = socket_open(&m_socket_data_address, 20, TRUE);
	m_socket_command = socket_open(&m_socket_command_address, 21, FALSE);

	return (m_socket_command == -1) | (m_socket_data == -1);
}

int socket_open(struct sockaddr_in *server_address, int port, int set_to_non_blocking)
{
	int flags;
	int socket_out;

	socket_out = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_out == -1)
	{
		perror("socket");
		return -1;
	}

	server_address->sin_family = AF_INET;
	server_address->sin_addr.s_addr = INADDR_ANY;
	server_address->sin_port = htons(port);
	if(bind(socket_out, (struct sockaddr*) server_address, sizeof(struct sockaddr_in)) < 0)
	{
		perror("bind");
		close(socket_out);
		return -1;
	}

	if(!set_to_non_blocking)
		return socket_out;

	flags = fcntl(socket_out, F_GETFL, 0);
	if(flags == -1)
	{
		perror("fcntl(F_GETFL)");
		close(socket_out);
		return -1;
	}

	if(fcntl(socket_out, F_SETFL, flags | O_NONBLOCK))
	{
		perror("fcntl(F_SETFL)");
		close(socket_out);
		return -1;
	}

	return socket_out;
}

void network_run()
{
	Client client;
	int read_size;
	char message[MAX_MESSAGE_SIZE];

	listen(m_socket_command, 3);

	while(TRUE)
	{
		printf("Waiting\n");
		client.address_size = sizeof(struct sockaddr_in);
		client.socket = accept(m_socket_command, (struct sockaddr*) &client.address, (socklen_t*) &client.address_size);
		if(client.socket < 0)
		{
			perror("accept");
			return;
		}

		printf("New client !\n");
		if(client.address.sin_addr.s_addr != client_ip_address.sin_addr.s_addr)
		{
			printf("Wrong client: %d\n", inet_ntoa(client_ip_address.sin_addr));
			close(client.socket);
			continue;
		}

		while((read_size = network_receive(client, message, MAX_MESSAGE_SIZE)) > 0)
		{
			printf("New packet: %.*s\n====\n", MAX_MESSAGE_SIZE, message);
		}
		
		if(read_size == -1)
		{
			perror("recv");
			return;
		}
		printf("The client ran out !\n");
	}
}

void network_exit()
{
	close(m_socket_data);
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