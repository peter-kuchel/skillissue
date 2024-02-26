#ifndef PIECETABLE_SKILLISSUE_H
#define PIECETABLE_SKILLISSUE_H

#define DEFAULT_PT_ELEM_SIZE    64

typedef enum {
    ORG, 
    ADD 
} piece_table_src; 

typedef struct {
    char* text; 
    size_t size; 
    size_t nt curr_pos;  /* for the add buffer */
} text_buffer; 

typedef struct{
    piece_table_src src; 
    int start; 
    int len; 
} piece_table_element; 

typedef struct {
    text_buffer* original_buffer; 
    text_buffer* add_buffer; 
    piece_table_element* table; 
    char start_ins_chr;
    size_t  start_ins_pos;
} piece_table; 


int init_piece_table(FILE* f, char* fn, piece_table* pt);
void empty_piece_table(piece_table* pt);

void resize_add_buffer(text_buffer* add_buff);

#endif 