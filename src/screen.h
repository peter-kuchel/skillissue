#ifndef SCREEN_SKILLISSUE_H 
#define SCREEN_SKILLISSUE_H 

#include <curses.h>
#include "piecetable.h"
#include "utils.h"

typedef struct {
    int rows; 
    int cols; 
} termw_info;

typedef struct {
    int x; 
    int y; 
} cursor_pos;

void intro_screen_no_file();
void render_screen(piece_table* pt);

#endif 