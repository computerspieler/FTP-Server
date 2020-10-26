#ifndef _FTP_H_
#define _FTP_H_

#include "network.h"

#define COMMAND_BUFFER_SIZE 4096

typedef struct Client Client;

typedef int(*CommandHandler)(Client*);
typedef struct Command Command;

struct Client
{
	Socket command;
	Socket data;

	char* message;
	char* arguments;
	int message_size;
};

struct Command
{
	char* name;
	int name_size;
	CommandHandler handler;
};

int ftp_new_connection_handler(Client*);
int ftp_packet_handler(Client*);

int ftp_send_response(Client*, int, char*, int);

#define HANDLER(name) int name##_handler(Client* client)
#define COMMAND(name) (Command) {#name, sizeof(#name) - 1, name##_handler}

HANDLER(USER);
HANDLER(SYST);
HANDLER(QUIT);
HANDLER(PORT);
HANDLER(TYPE);
HANDLER(MODE);
HANDLER(STRU);
HANDLER(RETR);
HANDLER(STOR);
HANDLER(NOOP);

static const Command commands[] = {
	COMMAND(USER),
	COMMAND(SYST),
	COMMAND(QUIT),
	COMMAND(PORT),
	COMMAND(TYPE),
	COMMAND(MODE),
	COMMAND(STRU),
	COMMAND(RETR),
	COMMAND(STOR),
	COMMAND(NOOP),
};

#define NB_COMMANDS (int)(sizeof(commands) / sizeof(Command))

#endif