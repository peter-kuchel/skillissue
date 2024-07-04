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

typedef struct {

    // size_t line_chr_ptr;
    int line_size;
    int line_pos; 

    int right_cutoff; 
     
} line_print;

typedef struct {
    pt_buffer_t* buff; 
    pt_entry ent; 

    size_t ent_end;
    size_t curr_start_ptr;
    int curr_ent;
} pt_track;

typedef struct {
    termw_info* tinfo_ptr;
    size_t top_view_chr;
    int top_view_ent;
    

    // sliding view window 
    int top_win;
    int bot_win; 
    int left_win; 
    int right_win;

    int total_line_chars;           // implement down the line

    int needs_render;               // determine if a screen render is necessary 
} line_view; 

void init_line_view(piece_table* pt, termw_info* tinfo, line_view* lv, line_handler* lh); 

void update_view_move_down(piece_table* pt, line_view* lv, cursor_pos* pos);
void update_view_move_up(piece_table* pt, line_view* lv, cursor_pos* pos);

void update_view_ins_nl(piece_table* pt, line_view* lv);
void update_view_del_nl(piece_table* pt, line_view* lv); 


void render_screen(piece_table* pt, line_view* lv);

// track_ptr->ent.start is the chr_ptr being tracked currently
#define CHR_IN_TRACK(track_ptr) \
        ( track_ptr->buff->text[track_ptr->ent.start] )

#endif 