#ifndef LOGGING_SKILLISSUE_H 
#define LOGGING_SKILLISSUE_H 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char* fname; 
    FILE* fstream;
} Logger; 

int log_to_file_stream(FILE* log_file, char* msg);
int log_to_file(char* log_file, char* msg);
int setup_logger(Logger* logger, char* log_file);
int teardown_logger(Logger* logger); 


#endif 