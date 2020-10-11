#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <arpa/inet.h>

#define COMMAND_BUFFER_SIZE 4096

typedef struct sockaddr_in Address;
typedef struct Socket Socket;

struct Socket
{
	int socket;
	socklen_t size;
	Address address;
};

int network_init();

Address network_convert_string_to_address(const char*);
int network_compare_address(Address, Address);

int network_open(Socket*, int, int);
int network_open_in_range(Socket*, int, int, int);

void network_close(Socket*);

int network_send(Socket, char*, int);
int network_receive(Socket, char*, int);

Socket network_wait_for_client(Socket*);
void network_listen(Socket*);

#endif