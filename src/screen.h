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

void render_screen(piece_table* pt);
int handle_side_movement(piece_table* pt, cursor_pos* pos, int dir);

#endif 