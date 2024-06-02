#ifndef LOGGING_SKILLISSUE_H 
#define LOGGING_SKILLISSUE_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LOG_FILE "skillissue_debug.log"
// #define LOG_TOGGLE 1

// #define LOG_INSERTS 1 
// #define LOG_MOVEMENT 0

typedef struct {
    char* fname; 
    FILE* fstream;
} Logger; 

Logger sk_logger; 

#define PBUF_SIZE 256

char pbuf[PBUF_SIZE];

int log_to_file(Logger* logger, char* msg);
int setup_logger(Logger* logger, char* log_file);
int teardown_logger(Logger* logger); 


#endif 