#include "pt_realloc.h"


void* handle_realloc(void* ptr, size_t new_cap){
    void* result = realloc(ptr, new_cap); 

    // allocation can fail if there isn't enough memory to realloc 
    if (result == NULL) exit(-1);

    return result;
}