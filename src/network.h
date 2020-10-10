#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <arpa/inet.h>

#define COMMAND_BUFFER_SIZE 4096

typedef struct Client Client;

struct Client
{
	int socket;
	socklen_t address_size;
	struct sockaddr_in address;
};

int network_init(const char*);
void network_run();
void network_exit();

int socket_open(struct sockaddr_in*, int, int);

int network_send(Client, char*, int);
int network_receive(Client, char*, int);

void network_new_connection(Client);

#endif