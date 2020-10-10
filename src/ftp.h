#ifndef _FTP_H_
#define _FTP_H_

int ftp_new_connection_handler();
int ftp_packet_handler(char*);

int ftp_send_response(int, char*, int);

#endif