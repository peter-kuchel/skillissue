#ifndef IMPORTS_SKILLISSUE_H
#define IMPORTS_SKILLISSUE_H

#include "cursor.h"
#include "piecetable.h"
#include "pt_delete.h"
#include "pt_insert.h"
#include "screen.h"
#include "utils.h"

typedef struct {
    FILE* f; 
} window_info;


typedef struct {
    int made_save;
    unsigned short mode;                    // mode(s) that the user is in       
    char cmd[10];                           // buffer to save the chars entered previously 
} usermode; 


#define USR_MODE_ESC  27
#define USR_BACKSPACE 127
// #define USER_DEL      

#define USR_MODE_INS  0x1
#define USR_MODE_SLT  0x2 
#define USR_MODE_CMD  0x4

#define TMP_ADD_BUF_SIZE 256

#define WRITE_SIZE    1024 

#endif 