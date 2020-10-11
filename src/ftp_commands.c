#include "ftp.h"
#include "ftp_commands.h"

HANDLER(USER)
{
    MESSAGE_UNUSED;
    ftp_send_response(client, 230, "User logged in", -1);
    return 0;
}

HANDLER(SYST)
{
    MESSAGE_UNUSED;
    ftp_send_response(client, 215, "UNIX TYPE: L8", -1);
    return 0;
}

HANDLER(QUIT)
{
    CLIENT_UNUSED;
    MESSAGE_UNUSED;
    return -1;
}