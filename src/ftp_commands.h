#ifndef _FTP_COMMANDS_H_
#define _FTP_COMMANDS_H_

#include "network.h"

typedef int(*CommandHandler)(Socket*, char*);
typedef struct Command Command;

struct Command
{
    char* name;
    int name_size;
    CommandHandler handler;
};

#define HANDLER(name) int name##_handler(Socket* client, char* message)
#define COMMAND(name) (Command) {#name, sizeof(#name) - 1, name##_handler}

#define MESSAGE_UNUSED (void) message;
#define CLIENT_UNUSED (void) client;

HANDLER(USER);
HANDLER(SYST);
HANDLER(QUIT);

static const Command commands[] = {
    COMMAND(USER),
    COMMAND(SYST),
    COMMAND(QUIT),
};

#define NB_COMMANDS (int)(sizeof(commands) / sizeof(Command))

#endif