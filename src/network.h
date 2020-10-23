#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <arpa/inet.h>

typedef struct sockaddr_in Address;
typedef struct Socket Socket;

struct Socket
{
	int socket;
	socklen_t size;
	Address address;
	int port;
};

int network_init();

Address network_convert_string_to_address(const char*);
void network_set_address_port(Address*, int);

int network_compare_address(Address, Address);

int network_open(Socket*, int);
int network_open_in_range(Socket*, int, int);

int network_send(Socket, char*, int);
int network_receive(Socket, char*, int);

int network_connect(Socket*);
void network_close(Socket*);

void network_listen(Socket*);
Socket network_wait_for_client(Socket*);

int network_set_socket_to_nonblocking(Socket*);

#endif