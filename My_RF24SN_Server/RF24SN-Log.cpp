#include "RF24SN-Log.h"

char* getTime() { //функция возвращает форматированную дату и время
    time_t now;
    struct tm *ptr;
    static char tbuf[64];
    bzero(tbuf,64);
    time(&now);
    ptr = localtime(&now);
    strftime(tbuf,64, "%b %e %H:%M:%S ", ptr);
   // strftime(tbuf,64, "%Y-%m-%e %H:%M:%S", ptr);
    return tbuf;
}

int writeLog(char msg[256]) { //функция записи строки в лог
    FILE * pLog;
    pLog = fopen("/home/alex/My_RF24SN_Server/daemon.log", "a");
    if(pLog == NULL) {
        return 1;
    }
    char str[312];
    bzero(str, 312);
    strcpy(str, getTime());
    strcat(str, msg);
    strcat(str, "\n");
    fputs(str, pLog);
    //fwrite(msg, 1, sizeof(msg), pLog);
    fclose(pLog);
    return 0;
}

int writeLog(char block[128], char msg[256]) { //функция записи строки в лог
    FILE * pLog;
    pLog = fopen("/home/alex/My_RF24SN_Server/daemon.log", "a");
    if(pLog == NULL) {
        return 1;
    }
    char str[440];
    bzero(str, 440);
    strcpy(str, getTime());
    strcat(str, block);
	strcat(str, ":");
	strcat(str, msg);
    strcat(str, "\n");
    fputs(str, pLog);
    //fwrite(msg, 1, sizeof(msg), pLog);
    fclose(pLog);
    return 0;
}
