#ifndef IMPORTS_SKILLISSUE_H
#define IMPORTS_SKILLISSUE_H

#include "piecetable.h"
#include "screen.h"

typedef struct {
    FILE* f; 
} window_info;


typedef struct {
    int made_save;
    unsigned short mode;                    // mode(s) that the user is in       
    char cmd[10];                           // buffer to save the chars entered previously 
} usermode; 


#define USR_MODE_ESC  27

#define USR_MODE_INS  0x1
#define USR_MODE_SLT  0x2 
#define USR_MODE_CMD  0x4

#endif 