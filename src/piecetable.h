#ifndef PIECETABLE_SKILLISSUE_H
#define PIECETABLE_SKILLISSUE_H

#define STARTING_ADD_BUF_SIZE   4096
#define DEFAULT_PT_ENT_SIZE    64

typedef enum {
    ORG, 
    ADD 
} pt_src_t; 

typedef struct {
    char* text; 
    size_t size; 
    size_t curr_pos;  /* for the add buffer */
} text_buffer; 

typedef struct{
    pt_src_t src; 
    size_t start; 
    size_t len; 
} pt_entry; 

typedef struct{
    pt_entry** entries; 
    int cap; 
    int curr_ent_num; 
} pt_table_t;

typedef struct{
    /* buffers to hold the text */

    text_buffer original; 
    text_buffer addition; 

    /* table along with undo / redo stacks */

    pt_table_t table; 
    pt_table_t redo_stack; 
    pt_table_t undo_stack; 

    /* information about the piece table state */

    pt_entry* curr_ent_ptr;                     // current ptr to the entry in the table 
    size_t curr_ch_pos;                         // currently pointed to char in the table 

    // size_t start_ins_pos;
    // char start_ins_chr;
} piece_table; 


int init_piece_table(FILE* f, char* fn, piece_table* pt);
void empty_piece_table(piece_table* pt);


#endif 