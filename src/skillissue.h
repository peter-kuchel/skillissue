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

typedef struct {
    FILE* f; 
} window_info;



#define USR_ESC  27
#define USR_BACKSPACE 127    

#define TMP_ADD_BUF_SIZE 256

#define WRITE_SIZE    1024 

#endif 