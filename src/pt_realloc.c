#include "pt_realloc.h"


void* handle_realloc(void* ptr, size_t new_cap){
    void* result = realloc(ptr, new_cap); 

    // allocation can fail if there isn't enough memory to realloc 
    if (result == NULL) exit(-1);

    return result;
}

void check_entries_size(piece_table* pt){
    if (pt->ent_num + 1 == pt->ent_cap){

        int new_cap = PT_INC_CAP(pt->ent_cap); 
        pt_entry* new_ptr = PT_REALLOC(pt_entry, pt->entries, new_cap); 

        pt->entries = new_ptr; 
        pt->ent_cap = new_cap;
        
    }
}

void check_additions_size(add_buffer_t* adds){
    if (adds->curr_pos + 1 == adds->buf.size){

        int new_size = PT_INC_CAP(adds->buf.size); 
        char* new_text_buf = PT_REALLOC(char, adds->buf.text, new_size); 

        adds->buf.text = new_text_buf; 
        adds->buf.size = new_size; 
    }
}

void check_pt_stack_size(pt_stack_t* pt_stack){
    if (pt_stack->ptr + 1 == pt_stack->cap){

        int new_cap = PT_INC_CAP(pt_stack->cap); 
        int* new_stack = PT_REALLOC(int, pt_stack->stack, new_cap);

        pt_stack->stack = new_stack;
        pt_stack->cap  = new_cap;
    }
}

int new_pt_entry(piece_table* pt){
    // check if entries need to be reallocd 
    check_entries_size(pt);

    pt->ent_num++; 
    int new_ent_pos = pt->ent_num; 
    
    // pt_entry* new_single_entry = ENT_AT_POS_ENTRIES(pt, new_ent_pos);
    pt_entry* new_single_entry = &(pt->entries[new_ent_pos]); 
    new_single_entry->src = ADD;
    new_single_entry->start = pt->addition.curr_pos; 
    new_single_entry->len = 0; 

    new_single_entry->left = NULL_ENT; 
    new_single_entry->right = NULL_ENT; 

    return new_ent_pos; 
}