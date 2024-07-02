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
} pt_track;

typedef struct {
    termw_info* tinfo_ptr;
    int top_view;
    int bot_view; 

    // sliding view window 
    int left_win; 
    int right_win;
    // int buff_offset; 

    int total_line_chars;           // implement down the line
} line_view; 

void init_line_view(termw_info* tinfo, line_view* lv, line_handler* lh); 
void update_view_ins_nl(line_handler* lh, line_view* lv);
void update_view_del_nl(line_handler* lh, line_view* lv); 
void render_screen(piece_table* pt, line_view* lv);

#define CHR_IN_TRACK(track_ptr) \
        ( track_ptr->buff->text[track_ptr->ent.start] )

#endif 