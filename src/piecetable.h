#ifndef PIECETABLE_SKILLISSUE_H
#define PIECETABLE_SKILLISSUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define STARTING_ADD_BUF_SIZE   4096
#define DEFAULT_PT_ENT_SIZE     64

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
} pt_entry; 

typedef struct{
    
    int*        organizer; 
    pt_entry*   entries;

    /* for organizer*/ 
    int org_cap; 
    int org_num;
    int org_head;                           
    int org_midd;
    int org_tail;

    /* for entries - as entries might reference ptrs in one of the various stacks */
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

    pt_buffer_t original; 
    add_buffer_t addition; 

    /* main table along with undo / redo and reclaim stacks */

    pt_table_t table; 
    pt_stack_t redo; 
    pt_stack_t undo; 
    pt_stack_t reclaim;                                     // exists to reclaim space from no longer valid redo ptrs 

    /* information about the piece table state */

    size_t      curr_chr_ptr;                               // currently pointed to char in the table 
    int         curr_org_ptr;                               // current ptr to position in the organizer

    int         insert_ready;


} piece_table; 

/* I know these macros are ugly af... */
#define GET_ENT_AT_POS(pt_ptr, pos) \
( pt_ptr->table.entries[pt_ptr->table.organizer[pos]] )

#define CURR_ORG_ENT(pt_ptr) \
( pt_ptr->table.entries[pt_ptr->table.organizer[pt_ptr->curr_org_ptr]] ) 

#define CURR_ORG_ENT_PTR(pt_ptr) ( &( CURR_ORG_ENT(pt_ptr) ) )

#define GET_PT_BUFF(pt_ptr, src) ( src == ORGN ? &(pt_ptr->original) : &(pt_ptr->addition.buf) )

#define CURR_PTR_AT_CHR(pt_ptr) \
( ((GET_PT_BUFF(pt_ptr, (CURR_ORG_ENT_PTR(pt_ptr))->src ))->text )[pt_ptr->curr_chr_ptr] )


void log_piece_table_current(Logger* logger, piece_table* pt);
int init_piece_table(FILE* f, char* fn, piece_table* pt);
void empty_piece_table(piece_table* pt);

char get_curr_char_by_entry(piece_table* pt, pt_entry* ent, size_t pos);
// int get_curr_pos(piece_table* pt);


void push_pt_stack_t(pt_stack_t* _stack, pt_entry* entry);
pt_entry* pop_pt_stack_t(pt_stack_t* _stack);


#endif 