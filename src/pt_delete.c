#include "pt_delete.h"


int delete_starting_from_end(piece_table* pt){

    int new_pt_entry = new_pt_insert_entry(pt);

    pt_entry* curr_ent = CURR_ORG_ENT_PTR(pt);
    pt_entry* ent = ENT_AT_POS_ENTRIES(pt, new_pt_entry);

    ent->len = curr_ent->len; 
    ent->src = curr_ent->src; 
    ent->start = curr_ent->start; 
    int old_ent = pt->table.organizer[pt->curr_org_ptr];

    pt_stack_t* _undo = &(pt->undo);
    push_pt_stack(_undo, old_ent);

    pt->curr_org_ptr = pt->table.org_tail;
    pt->curr_del_ent = new_pt_entry;
    pt->curr_del_org = pt->curr_org_ptr;

    #ifdef DEBUG_DELETE 
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Delete - From End]: ent: %d, ent_org:%d chr_ptr: %ld\n", 
            pt->curr_del_ent, pt->curr_del_org, pt->curr_chr_ptr);
        log_to_file(&sk_logger, pbuf); 
    #endif 
     

    return 0; 
}

int delete_from_side(piece_table* pt){

    int new_pt_entry = new_pt_insert_entry(pt);
    int left_pos = pt->curr_org_ptr - 1;
    int left_ent_pos = pt->table.organizer[left_pos];
    
    pt_entry* left_ent = ENT_AT_POS_ENTRIES(pt, left_ent_pos);
    pt_entry* ent = ENT_AT_POS_ENTRIES(pt, new_pt_entry);

    ent->len = left_ent->len; 
    ent->src = left_ent->src; 
    ent->start = left_ent->start; 

    pt_stack_t* _undo = &(pt->undo);
    push_pt_stack(_undo, left_pos);

    pt->curr_del_org = left_pos; 
    pt->curr_del_ent = new_pt_entry; 

    #ifdef DEBUG_DELETE 
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Delete - Side]: ent: %d, ent_org:%d chr_ptr: %ld\n", 
            pt->curr_del_ent, pt->curr_del_org, pt->curr_chr_ptr);
        log_to_file(&sk_logger, pbuf); 
    #endif 

    return 0; 
}

int delete_in_middle(piece_table* pt){
    
    int new_del_ent = new_pt_insert_entry(pt);
    int new_split_ent = new_pt_insert_entry(pt);

    // int del_org_pos = pt->curr_org_ptr - 1;

    pt_entry* del_ent = ENT_AT_POS_ENTRIES(pt, new_del_ent);
    pt_entry* split_ent = ENT_AT_POS_ENTRIES(pt, new_split_ent);
    pt_entry* curr_ent = CURR_ORG_ENT_PTR(pt);

    pt_stack_t* _undo = &(pt->undo);
    push_pt_stack(_undo, ENT_POS_FROM_ORG_PTR(pt));

    del_ent->start = curr_ent->start; 
    del_ent->src = curr_ent->src; 
    del_ent->len = (pt->curr_chr_ptr) - curr_ent->start;

    split_ent->start = pt->curr_chr_ptr; 
    split_ent->src = curr_ent->src; 
    split_ent->len = (curr_ent->len) - del_ent->len; 
    
    // shift to make space for the del ent
    int left_size = pt->curr_org_ptr - pt->table.org_head; 
    int right_size = pt->table.org_tail = pt->curr_org_ptr; 

    int going_left = (left_size < right_size);
    int shift_start = going_left ? pt->table.org_head : pt->table.org_tail; 
    int shift_end = going_left ? pt->curr_org_ptr - 1 : pt->curr_org_ptr;

    if (going_left){
        shift_organizer_left(pt, shift_start, shift_end);
        pt->curr_del_org = pt->curr_org_ptr - 1;  
    } else {
        shift_organizer_right(pt, shift_start, shift_end);
        pt->curr_del_org = pt->curr_org_ptr; 
        pt->curr_org_ptr++; 
    }

    pt->table.organizer[pt->curr_org_ptr] = new_split_ent;
    pt->table.organizer[pt->curr_del_org] = new_del_ent;
    pt->curr_del_ent = new_del_ent; 

    #ifdef DEBUG_DELETE 
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Delete - Middle]: ent: %d, ent_org:%d chr_ptr: %ld\n", 
            pt->curr_del_ent, pt->curr_del_org, pt->curr_chr_ptr);
        log_to_file(&sk_logger, pbuf); 
    #endif 

    return 0; 
}

int delete_manager(piece_table* pt, cursor_pos* curs_pos){

    pt_entry* del_ent; 
    // delete setup - handle the 2 delete cases with the setup first 
    if (pt->curr_del_ent < 0 && pt->curr_del_org < 0){

        #ifdef DEBUG_DELETE
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, "[Delete]: Need to create new delete entry\n");
            log_to_file(&sk_logger, pbuf); 
        #endif 

        pt_entry* curr_ent = CURR_ORG_ENT_PTR(pt);

        // if at the very front of the file, nothing can be deleted, return 
        if (
            pt->table.org_head == pt->curr_org_ptr && 
            pt->curr_chr_ptr == curr_ent->start
        ){

            #ifdef DEBUG_DELETE
                memset(pbuf, 0, PBUF_SIZE);
                sprintf(pbuf, "[Delete]: At very beginning - nothing to delete\n");
                log_to_file(&sk_logger, pbuf); 
            #endif

            return 0;
        } 
        
        
        // if at the very end of the file 
        if (
            pt->table.org_tail == pt->curr_org_ptr && 
            pt->curr_chr_ptr == (curr_ent->start + curr_ent->len)
        ){
                delete_starting_from_end(pt);
        
        // start deleting at the ent left of the current ent
        } else if (pt->curr_chr_ptr == curr_ent->start){
            delete_from_side(pt);

        // somewhere in the middle of the current ent 
        } else {
            delete_in_middle(pt);
        }
        

    // else check current if delete ent has been exhuasted 
    } 
    
    del_ent = ENT_AT_POS_ENTRIES(pt, pt->curr_del_ent);
    if (del_ent->len == 0){

        #ifdef DEBUG_DELETE
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, "[Delete]: Exhuasted current delete ent\n");
            log_to_file(&sk_logger, pbuf); 
        #endif 

        // if we are at the very end with current del ent, then just return 
        if (pt->table.org_head == pt->curr_del_org && pt->curr_chr_ptr == 0){
            return 0;
        }

        // if the len of an entry is zero, use the left most to it as new delete ent 
        // shift everything to remove its place
        int left_size = pt->curr_del_org - pt->table.org_head; 
        int right_size = pt->table.org_tail - pt->curr_del_org; 

        int going_right = left_size < right_size;
        int shift_start = going_right ? pt->curr_org_ptr - 1 : pt->curr_org_ptr + 1;
        int shift_end = going_right ? pt->table.org_head : pt->table.org_tail; 

        if (going_right){
            shift_organizer_right(pt, shift_start, shift_end);
        } else {
            shift_organizer_left(pt, shift_start, shift_end);
        }

        del_ent = ENT_AT_POS_ENTRIES(pt, pt->curr_del_ent);
    
    }

    // else continue deleting from the current ent (if possible) and update necessary values

    #ifdef DEBUG_DELETE
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Delete before]: ent: %d, len: %ld, chr_ptr: %ld\n", 
            pt->curr_del_ent, del_ent->len, pt->curr_chr_ptr);
        log_to_file(&sk_logger, pbuf); 
    #endif 

    del_ent->len--; 
    pt->curr_chr_ptr--; 
    curs_pos->x--; 

    #ifdef DEBUG_DELETE
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Delete after]: ent: %d, len: %ld, chr_ptr: %ld\n", 
            pt->curr_del_ent, del_ent->len, pt->curr_chr_ptr);
        log_to_file(&sk_logger, pbuf); 
    #endif 

    #ifdef DEBUG_PT 
        log_piece_table_current(&sk_logger, pt);
    #endif 

    return 0; 
}