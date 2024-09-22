#ifndef IMPORTS_SKILLISSUE_H
#define IMPORTS_SKILLISSUE_H

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

// #include <curses.h>

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


#define USR_ESC  27
#define USR_BACKSPACE 127    

typedef enum {

    MODE_INSERT = 0x1,
    MODE_SAVE   = 0x2,

} usr_mode;

#define TMP_ADD_BUF_SIZE 256

#define WRITE_SIZE    1024 

#endif 