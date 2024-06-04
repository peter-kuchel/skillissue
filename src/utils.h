#ifndef UTILS_SKILLISSUE_H 
#define UTILS_SKILLISSUE_H 

#include <sys/stat.h>
#include <stdio.h>

#include "logging.h"

int file_exists(char* fname);
size_t file_size(char* fname);

#define INC_CAP(cap) (cap << 1)

#define REALLOC(type, ptr, new_cap) \
    ( (type*)handle_realloc(ptr, sizeof(type) * new_cap) )

void* handle_realloc(void* ptr, size_t new_cap);

// from Understanding and Using C Pointers by Richard Reese 
void saferFree(void** pp);
#define safeFree(p) saferFree((void**)&(p)) 


#endif