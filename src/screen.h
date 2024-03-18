#ifndef SCREEN_SKILLISSUE_H 
#define SCREEN_SKILLISSUE_H 

#include <curses.h>
#include "piecetable.h"

typedef struct {
    int rows; 
    int cols; 
} termw_info;

void intro_screen_no_file();
void render_screen(piece_table* pt);

#endif 