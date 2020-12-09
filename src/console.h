#ifndef __CONSOLE_H__
#define __CONSOLE_H__

int console_init();
int console_close();

int console_write(char*, ...);
int console_yes_or_no();

#endif