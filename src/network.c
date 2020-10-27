#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "ftp.h"
#include "network.h"
#include "typedef.h"

int network_init()
{
	return 0;
}

Address network_convert_string_to_address(const char* address_str)
{
	Address output;
	output.sin_family = AF_INET;
	inet_aton(address_str, &output.sin_addr);

	return output;
}

char* network_convert_address_to_string(Address address)
{
	return inet_ntoa(address.sin_addr);
}

int network_compare_address(Address address1, Address address2)
{
	return address1.sin_addr.s_addr == address2.sin_addr.s_addr;
}


void network_set_address_port(Address* address, int port)
{
	if(address != NULL)
		address->sin_port = htons(port);
}

int network_open(Socket* sock, int port)
{
	sock->port = port;
	sock->socket = socket(AF_INET, SOCK_STREAM, 0);
	if(sock->socket == -1)
		return -1;

	sock->address.sin_family = AF_INET;
	sock->address.sin_addr.s_addr = INADDR_ANY;
	sock->address.sin_port = htons(port);
	if(bind(sock->socket, (struct sockaddr*) &sock->address, sizeof(struct sockaddr_in)) < 0)
	{
		network_close(sock);
		return -2;
	}

	return 0;
}

Socket network_wait_for_client(Socket* sock)
{
	Socket new_client;

	printf("Waiting for clients\n");
	new_client.size = sizeof(struct sockaddr_in);
	new_client.socket = accept(sock->socket, (struct sockaddr*) &new_client.address, (socklen_t*) &new_client.size);
	if(new_client.socket < 0)
	{
		perror("accept");
		return new_client;
	}

	return new_client;
}

int network_send(Socket sock, char* buffer, int buffer_size)
{
	return send(sock.socket, buffer, buffer_size, 0);
}

int network_receive(Socket sock, char* buffer, int buffer_size)
{
	return recv(sock.socket, buffer, buffer_size, 0);
}

int network_open_in_range(Socket* sock, int port_begin, int port_end)
{
	int port;
	for(port = port_begin; sock->socket == 0; port ++)
	{
		if(port >= port_end)
		{
			printf("No available port between %i and %i\n", port_begin, port_end);
			return -1;
		}

		if(!network_open(sock, port))
			break;

		if(sock->socket == -2 && errno == EADDRINUSE)
			continue;

		perror("Data socket");
		return -1;
	}

	return 0;
}

void network_close(Socket* sock)
{
	if(close(sock->socket))
		perror("close");
	sock->socket = 0;
}


void network_listen(Socket* sock)
{
	listen(sock->socket, 3);
}


int network_connect(Socket* sock)
{
	sock->socket = socket(AF_INET, SOCK_STREAM, 0);
	if(sock->socket == -1)
		return -1;

	if(connect(sock->socket, (struct sockaddr*) &sock->address, sizeof(struct sockaddr_in)))
	{
		network_close(sock);
		return -2;
	}

	return 0;
}

int network_set_socket_to_nonblocking(Socket* sock)
{
	int flags = fcntl(sock->socket, F_GETFL, 0);
	if(flags == -1)
	{
		network_close(sock);
		return -3;
	}

	if(fcntl(sock->socket, F_SETFL, flags | O_NONBLOCK))
	{
		network_close(sock);
		return -4;
	}

	return 0;
}