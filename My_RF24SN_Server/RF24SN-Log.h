#ifndef RF24SN_LOG_h
#define RF24SN_LOG_h

#include <string.h>
#include <stdio.h>
#include <time.h>

int writeLog(char msg[256]);
int writeLog(char block[128], char msg[256]);
char* getTime(void);


#endif