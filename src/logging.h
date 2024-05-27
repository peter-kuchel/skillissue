#ifndef LOGGING_SKILLISSUE_H 
#define LOGGING_SKILLISSUE_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_FILE "skillissue_log_file.log"
#define LOG_TOGGLE 1

typedef struct {
    char* fname; 
    FILE* fstream;
} Logger; 

Logger sk_logger; 
// int pbuf_size = 256;
#define PBUF_SIZE 256

char pbuf[PBUF_SIZE];

int log_to_file(Logger* logger, char* msg);
int setup_logger(Logger* logger, char* log_file);
int teardown_logger(Logger* logger); 


#endif 