#ifndef _FTP_H_
#define _FTP_H_

#include "network.h"

typedef struct Client Client;

typedef int(*CommandHandler)(Client*);
typedef struct Command Command;
typedef enum TrasmissionType TrasmissionType;

enum TrasmissionType
{
    TRANSMISSION_BINARY,
    TRANSMISSION_ASCII
};

struct Client
{
	Socket command;
	Socket data;

	char* message;
	char* arguments;
	int arguments_size;
	int message_size;

    TrasmissionType transmission_type;
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
int ftp_open_data_socket(Client*);

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
HANDLER(LIST);

static Command commands[] = {
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
	COMMAND(LIST),
};

#define NB_COMMANDS (int)(sizeof(commands) / sizeof(Command))

#endif
