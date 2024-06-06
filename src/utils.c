#include <stdio.h>
#include <stdlib.h>

#include "utils.h"


int file_exists(char* fname){
    struct stat fin;

    if ( stat(fname, &fin) < 0) return -1;

     return 0; 
}

size_t file_size(char* fname){
    struct stat fin;

     if ( stat(fname, &fin) < 0){
        printf("[ERROR]: could not get info on file %s\n", fname);
        return 0;
     }

     return (size_t)fin.st_size;
 
}

// from Understanding and Using C Pointers by Richard Reese 
void saferFree(void**pp){
	if (pp != NULL && *pp != NULL){
		free(*pp);
		*pp = NULL; 
	}
}