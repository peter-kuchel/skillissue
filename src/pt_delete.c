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

    return 0; 
}

int delete_in_middle(piece_table* pt){
    
    int new_del_ent = new_pt_insert_entry(pt);
    int new_split_ent = new_pt_insert_entry(pt);

    int del_org_pos = pt->curr_org_ptr - 1;

    pt_entry* del_ent = ENT_AT_POS_ENTRIES(pt, new_del_ent);
    pt_entry* split_ent = ENT_AT_POS_ENTRIES(pt, new_split_ent);
    pt_entry* curr_ent = CURR_ORG_ENT_PTR(pt);

    pt_stack_t* _undo = &(pt->undo);
    push_pt_stack(_undo, ENT_POS_FROM_ORG_PTR(pt));

    del_ent->start = curr_ent->start; 
    del_ent->src = ADD; 
    del_ent->len = (pt->curr_chr_ptr) - curr_ent->start;

    split_ent->start = pt->curr_chr_ptr; 
    split_ent->src = ADD; 
    split_len->len = (curr_ent->len) - del_ent->len; 
    
    // shift 

    pt->curr_del_org = del_org_pos;
    pt->curr_del_ent = new_del_ent; 

    return 0; 
}

int delete_manager(piece_table* pt, cursor_pos* curs_pos){

    pt_entry* del_ent; 
    // delete setup - handle the 2 delete cases with the setup first 
    if (pt->curr_del_ent < 0 && pt->curr_del_org < 0){

        
        pt_entry* curr_ent = CURR_ORG_ENT_PTR(pt);

        // if at the very front of the file, nothing can be deleted, return 
        if (
            pt->table.org_head == pt->curr_org_ptr && 
            pt->curr_chr_ptr == curr_ent->start
        )
            return 0; 
        
        
        // if at the very end of the file 
        if (
            pt->table.org_tail == pt->curr_org_ptr && 
            pt->curr_chr_ptr == (curr_ent->start + curr_ent->len)
        ){
                delete_starting_from_end(pt);
        
        // at the left of the current ent
        } else if (pt->curr_chr_ptr == curr_ent->start){
            delete_from_side(pt);
        // somewhere in the middle of the current ent 
        } else {
            delete_in_middle(pt);
        }
        

    // else check current if delete ent has been exhuasted 
    } else if (del_ent->len == 0){

    
    // else continue deleting from the current ent (if possible) and update necessary values
    }
        
    del_ent = ENT_AT_POS_ENTRIES(pt, pt->curr_del_ent);
    del_ent->len--; 
    pt->curr_chr_ptr--; 
    curs_pos->x--; 


    return 0; 
}