#ifndef PIECETABLE_SKILLISSUE_H
#define PIECETABLE_SKILLISSUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define STARTING_ADD_BUF_SIZE   4096
#define DEFAULT_PT_ENT_SIZE     64

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
    
    int*        organizer; 
    pt_entry*   entries;

    /* for organizer*/ 
    int org_cap; 
    int org_num;
    int org_head;                           
    int middle;

    /* for entries - as entries might reference ptrs in the one of the various stacks */
    int ent_cap; 
    int ent_num; 
} pt_table_t;

typedef struct{
    int* stack; 
    int cap; 
    int ptr; 
} pt_stack_t; 

typedef struct{
    /* buffers to hold the text */

    text_buffer original; 
    text_buffer addition; 

    /* table along with undo / redo stacks and reclaim */

    pt_table_t table; 
    pt_stack_t redo; 
    pt_stack_t undo; 
    pt_stack_t reclaim;                                     // exists to reclaim space from no longer valid redo ptrs 

    /* information about the piece table state */

    int         curr_ent_ptr;                               // current ptr to the entry in the table 
    size_t      curr_chr_ptr;                               // currently pointed to char in the table 

} piece_table; 


int init_piece_table(FILE* f, char* fn, piece_table* pt);
void empty_piece_table(piece_table* pt);

void push_pt_stack_t(pt_stack_t* _stack, pt_entry* entry);
pt_entry* pop_pt_stack_t(pt_stack_t* _stack);


#endif 