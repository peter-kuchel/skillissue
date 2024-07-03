#ifndef PIECETABLE_SKILLISSUE_H
#define PIECETABLE_SKILLISSUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "utils.h"

#define STARTING_ADD_BUF_SIZE   4096
#define DEFAULT_PT_ENT_SIZE     64
#define DEFAULT_LINES_SIZE      512

#define NULL_ENT -1
#define NULL_LINE -1

typedef struct{
    int* stack; 
    int cap; 
    int ptr; 
} pt_stack_t;

typedef struct {
    int next_line; 
    int prev_line; 
    int line_size; 
} line; 

typedef struct {

    pt_stack_t line_reclaim; 
    line *lines;
    int cap;
    int size; 

    int top_line; 
    int curr_line;
    int bottom_line; 

    int line_number;                    // actual line number of the current line
    // int nl_side_hit;

    int col_mem;                        // remember which col the cursor is on
    // int top_view;                       // top line in view in the editor
    // int bot_view;                       // bottom line in view in the editor

} line_handler; 

typedef enum {
    ORGN = 0, 
    ADD = 1 
} pt_src_t; 

typedef struct {
    char* text; 
    size_t size;
} pt_buffer_t; 

typedef struct {

    pt_buffer_t buf;  

    size_t curr_pos;  
    size_t saved_pos;
    
    int write_active;
} add_buffer_t; 

typedef struct{
    pt_src_t src; 
    size_t start; 
    size_t len; 

    int left; 
    int right; 
} pt_entry;  

typedef struct{
    /* buffers to hold the text */

    pt_buffer_t original; 
    add_buffer_t addition; 

    /* main table along with undo / redo and reclaim stacks */

    pt_entry* entries;                                      //   
    line_handler lh;                                        // handle new lines

    // pt_table_t table; 
    pt_stack_t redo; 
    pt_stack_t undo; 
    pt_stack_t reclaim;                                     // exists to reclaim space from no longer valid redo ptrs 

    /* information about the piece table state */

    size_t      curr_chr_ptr;                               // currently pointed to char in the table
    size_t      prev_chr_ptr;                               // previously pointed to char (to handle moving between \n) 
    int         curr_ent_ptr;                               // index of currently pointed to entry 
    int         ent_head;                                   // head in the list
    int         ent_tail;                                   // tail in the list 

    int         ent_cap;                                    // capacity of the array
    int         ent_num;                                    // current number of entries in the array

    int         curr_del_ent;                               // index of current entry handling deletes
    int         curr_ins_ent;                               // index of current entry handling inserts

} piece_table; 


// get the underlying pt_buffer_t depending on the origin given
#define GET_PT_BUFF(pt, src) \
    ( src == ORGN ? &(pt->original) : &(pt->addition.buf) )

#define PTR_AT_CHR(pt, pos) \
    ( (GET_PT_BUFF(pt, (pt->entries[pt->curr_ent_ptr]).src )->text)[pos] )

#define LH_CURR_LINE(pt) \
    ( &(pt->lh.lines[pt->lh.curr_line]) )

#define CURR_LINE_SIZE(pt) \
    ( (LH_CURR_LINE(pt))->line_size )    

#define ENT_AT_POS(pt, pos) \
    ( &(pt->entries[pos]) )

#define LINE_AT_POS(lh, pos) \
    ( &(lh->lines[pos]) )

void log_piece_table_current(Logger* logger, piece_table* pt);
int init_piece_table(FILE* f, char* fn, piece_table* pt);
void empty_piece_table(piece_table* pt);


#endif 