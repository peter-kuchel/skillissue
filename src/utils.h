#ifndef UTILS_SKILLISSUE_H 
#define UTILS_SKILLISSUE_H 

#include <sys/stat.h>
#include <stdio.h>

#define LOG_FILE "skillissue_log_file.log"

int file_exists(char* fname);
size_t file_size(char* fname);


// from Understanding and Using C Pointers by Richard Reese 
void saferFree(void** pp);
#define safeFree(p) saferFree((void**)&(p)) 


#endif