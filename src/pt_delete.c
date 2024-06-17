#include "pt_delete.h"


static int delete_starting_from_end(piece_table* pt){

    int new_del_ent = new_pt_entry(pt);

    int old_ent = pt->curr_ent_ptr;
    pt_entry* curr_ent = &(pt->entries[old_ent]);
    pt_entry* del_ent = &(pt->entries[new_del_ent]);

    int old_left = curr_ent->left; 

    del_ent->len = curr_ent->len; 
    del_ent->src = curr_ent->src; 
    del_ent->start = curr_ent->start; 

    del_ent->right = curr_ent->right; 
    del_ent->left = curr_ent->left; 

    push_pt_stack(&(pt->undo), old_ent);

    if (pt->ent_head == old_ent){
        pt->ent_head = new_del_ent;
    } else {
        pt_entry* old_left_ent = &(pt->entries[old_left]);
        old_left_ent->right = new_del_ent;
    }
    
    pt->ent_tail = new_del_ent; 
    pt->curr_ent_ptr = new_del_ent;

    pt->curr_del_ent = new_del_ent;


    #ifdef DEBUG_DELETE 
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Delete - From End]: del_ent: %d, chr_ptr: %ld\n", 
            pt->curr_del_ent, pt->curr_chr_ptr);
        log_to_file(&sk_logger, pbuf); 
    #endif 
     

    return 0; 
}

static int delete_from_side(piece_table* pt){

    int new_del_ent = new_pt_entry(pt);

    pt_entry* curr_ent = &(pt->entries[pt->curr_ent_ptr]);
    pt_entry* del_ent = &(pt->entries[new_del_ent]);

    int left_of_curr = curr_ent->left;
    pt_entry* left_ent = &(pt->entries[left_of_curr]);

    int left_left_of_curr = left_ent->left; 

    del_ent->len = left_ent->len; 
    del_ent->src = left_ent->src; 
    del_ent->start = left_ent->start; 

    del_ent->left = left_ent->left; 
    del_ent->right = left_ent->right; 

    curr_ent->left = new_del_ent; 

    push_pt_stack(&(pt->undo), left_of_curr);

    if (left_of_curr == pt->ent_head){
        pt->ent_head = new_del_ent;
    } else {
        pt_entry* left_left = &(pt->entries[left_left_of_curr]);
        left_left->right = new_del_ent;
    }

    pt->curr_del_ent = new_del_ent; 

    #ifdef DEBUG_DELETE 
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Delete - Side]: del_ent: %d, chr_ptr: %ld\n", 
            pt->curr_del_ent, pt->curr_chr_ptr);
        log_to_file(&sk_logger, pbuf); 
    #endif 

    return 0; 
}

static int delete_in_middle(piece_table* pt){
    
    pt_entry* _side_ent; 
    int new_del_ent = new_pt_entry(pt);
    int new_split_ent = new_pt_entry(pt);

    pt_entry* old_curr_ent = &(pt->entries[pt->curr_ent_ptr]);

    int old_left = old_curr_ent->left; 
    int old_right = old_curr_ent->right;

    pt_entry* del_ent = &(pt->entries[new_del_ent]);
    pt_entry* split_ent = &(pt->entries[new_split_ent]);
    

    push_pt_stack(&(pt->undo), pt->curr_ent_ptr);

    del_ent->start = old_curr_ent->start; 
    del_ent->src = old_curr_ent->src; 
    del_ent->len = (pt->curr_chr_ptr) - old_curr_ent->start; 

    del_ent->left = old_curr_ent->left; 
    del_ent->right = new_split_ent;

    split_ent->start = pt->curr_chr_ptr;
    split_ent->src = old_curr_ent->src;
    split_ent->len = (old_curr_ent->len) - del_ent->len; 

    split_ent->left = new_del_ent;
    split_ent->right = old_curr_ent->right;

    // if we are at the tail
    if (pt->ent_tail == pt->curr_ent_ptr){
        pt->ent_tail = new_split_ent;
    } else {
        _side_ent = &(pt->entries[old_right]);
        _side_ent->left = new_split_ent;
    }
    
    // if we are at the head 
    if (pt->ent_head == pt->curr_ent_ptr){
        pt->ent_head = new_del_ent;
    } else {
        _side_ent = &(pt->entries[old_left]);
        _side_ent->right = new_del_ent;
    }

    pt->curr_del_ent = new_del_ent; 
    pt->curr_ent_ptr = new_split_ent; 

    #ifdef DEBUG_DELETE 
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Delete - Middle]: del_ent: %d, chr_ptr: %ld\n", 
            pt->curr_del_ent, pt->curr_chr_ptr);
        log_to_file(&sk_logger, pbuf); 
    #endif 

    return 0; 
}

static int delete_curr_exhuasted(piece_table* pt){
    #ifdef DEBUG_DELETE
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Delete]: Exhuasted current delete ent\n");
        log_to_file(&sk_logger, pbuf); 
    #endif 

    // if we are at the very end with current del ent, then return and set necessary variables 
    if (pt->ent_head == pt->curr_del_ent){

        #ifdef DEBUG_DELETE
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, "[Delete]: Exhausted at very beginning\n");
            log_to_file(&sk_logger, pbuf);
        #endif 

        pt->ent_head = pt->curr_ent_ptr; 

        pt_entry* head_ent = &(pt->entries[pt->ent_head]);
        head_ent->left = NULL_ENT;

        pt->curr_del_ent = NULL_ENT; 
         
        return 1;
    }

    int curr_del = pt->curr_del_ent;
    int new_del = new_pt_entry(pt);

    pt_stack_t* _undo = &(pt->undo);

    
    pt_entry* curr_del_ent = &(pt->entries[curr_del]);
    int del_left = curr_del_ent->left; 

    pt_entry* del_left_ent = &(pt->entries[del_left]);
    pt_entry* new_del_ent = &(pt->entries[new_del]);
    pt_entry* curr_ent = &(pt->entries[pt->curr_ent_ptr]);

    int del_left_left = del_left_ent->left; 

    new_del_ent->start = del_left_ent->start; 
    new_del_ent->src = del_left_ent->src; 
    new_del_ent->len = del_left_ent->len; 

    new_del_ent->left = del_left_ent->left; 
    new_del_ent->right = curr_del_ent->right;

    curr_ent->left = new_del;

    push_pt_stack(_undo, curr_del);
    push_pt_stack(_undo, del_left);

    if (curr_del == pt->ent_tail){
        pt->ent_tail = new_del;
        pt->curr_ent_ptr = new_del;
    }

    if (del_left == pt->ent_head){
        pt->ent_head = new_del; 
    } else {
        pt_entry* left_left = &(pt->entries[del_left_left]);
        left_left->right = new_del;
    }

    pt->curr_del_ent = new_del;

    #ifdef DEBUG_DELETE 
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Delete Exhuast Replace] old: %d, replacement: %d\n", 
            del_left ,new_del);
        log_to_file(&sk_logger, pbuf); 
    #endif

    return 0; 
}

static int handle_new_line_delete(piece_table* pt, cursor_pos* curs_pos){

    return 0; 
}

int delete_manager(piece_table* pt, cursor_pos* curs_pos, int key_pressed){

    // handle delete key presses 
    if (key_pressed == KEY_DC){

        return 0; 
    }

    pt_entry* del_ent; 
    // delete setup - handle the 2 delete cases with the setup first 
    if (pt->curr_del_ent < 0){

        #ifdef DEBUG_DELETE
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, "[Delete]: Need to create new delete entry\n");
            log_to_file(&sk_logger, pbuf); 
        #endif 

        // pt_entry* curr_ent = CURR_ORG_ENT_PTR(pt);
        pt_entry* curr_ent = &(pt->entries[pt->curr_ent_ptr]);

        // if at the very front of the file, nothing can be deleted, return 
        if (
            pt->ent_head == pt->curr_ent_ptr && 
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
            pt->ent_tail == pt->curr_ent_ptr && 
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
    } 
    
    // del_ent = ENT_AT_POS_ENTRIES(pt, pt->curr_del_ent);
    del_ent = &(pt->entries[pt->curr_del_ent]);

    // else continue deleting from the current ent (if possible) and update necessary values

    #ifdef DEBUG_DELETE
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Delete before]: ent: %d, len: %ld, chr_ptr: %ld\n", 
            pt->curr_del_ent, del_ent->len, pt->curr_chr_ptr);
        log_to_file(&sk_logger, pbuf); 
    #endif 

    
    if (pt->curr_del_ent == pt->ent_tail)
        pt->curr_chr_ptr--;
    
    // get the character before decrementing the len 
    int char_pos = (del_ent->start + del_ent->len) - 1;
    char _c = (( GET_PT_BUFF(pt, del_ent->src) )->text)[char_pos];
    del_ent->len--; 

    // check if a new line was removed 
    if (_c == '\n'){
        handle_new_line_delete(pt, curs_pos);
    } else {
        line* curr_line = LH_CURR_LINE(pt);
        curr_line->line_size--;
        curs_pos->x--;
    } 

    #ifdef DEBUG_DELETE
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Delete after]: ent: %d, len: %ld, chr_ptr: %ld\n", 
            pt->curr_del_ent, del_ent->len, pt->curr_chr_ptr);
        log_to_file(&sk_logger, pbuf); 
    #endif 

    // else check current if delete ent has been exhuasted 
    if (del_ent->len == 0){

        delete_curr_exhuasted(pt);

        // if (at_end) return 0; 

        // del_ent = &(pt->entries[pt->curr_del_ent]);
        // del_ent = ENT_AT_POS_ENTRIES(pt, pt->curr_del_ent);
    }

    #ifdef DEBUG_PT 
        log_piece_table_current(&sk_logger, pt);
    #endif 

    return 0; 
}