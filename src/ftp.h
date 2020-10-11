#ifndef _FTP_H_
#define _FTP_H_

#include "network.h"

#define COMMAND_BUFFER_SIZE 4096

int ftp_new_connection_handler(Socket*);
int ftp_packet_handler(Socket*, char*);

int ftp_send_response(Socket*, int, char*, int);

#endif