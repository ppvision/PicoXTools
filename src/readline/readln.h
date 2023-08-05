#ifndef _DREAD_LINE_H
#define _DREAD_LINE_H
#include <stdio.h>
#include <stdlib.h>
#include "tty_uart.h"

#define ETX 0x03  // Ctrl + C, SIGINT


void savehist();
char* dgreadln(char* buffer, int mnt, char* prom);


#endif
