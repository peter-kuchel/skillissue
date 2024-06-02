#include "pt_insert.h"

static int create_from_empty(piece_table* pt){

    int new_ent_pos = new_pt_entry(pt);

    push_pt_stack(&(pt->undo), pt->curr_ent_ptr);

    pt->curr_ent_ptr = new_ent_pos;
    pt->curr_ins_ent = new_ent_pos; 

    pt->ent_tail = new_ent_pos;
    pt->ent_head = new_ent_pos;
    // pt->table.organizer[pt->curr_org_ptr] = new_ent_pos; 
    // pt->curr_ins_ent = new_ent_pos; 
    // pt->curr_ins_org = pt->curr_org_ptr;

    #ifdef DEBUG_INSERT
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Empty insert]: ent pos: %d\n", new_ent_pos);
        log_to_file(&sk_logger, pbuf);
    #endif 
    
    return 0; 
}

static int create_end_insert(piece_table* pt, int very_end){

    pt_entry *curr_ent, *new_ent;
    int new_ent_pos = new_pt_entry(pt);

    curr_ent = &(pt->entries[pt->curr_ent_ptr]);
    new_ent = &(pt->entries[new_ent_pos]);

    // depending on the end - adjust values
    if (very_end){

        pt->curr_chr_ptr = pt->addition.curr_pos; 

        curr_ent->right = new_ent_pos;
        new_ent->left = pt->curr_ent_ptr;

        pt->ent_tail = new_ent_pos; 
        pt->curr_ent_ptr = new_ent_pos; 
    } else {

        curr_ent->left = new_ent_pos; 
        new_ent->right = pt->curr_ent_ptr; 

        pt->ent_head = new_ent_pos;
    }

    pt->curr_ins_ent = new_ent_pos; 

    #ifdef DEBUG_INSERT
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Ends insert]: new_ent_pos: %d\n", new_ent_pos);
        log_to_file(&sk_logger, pbuf);
    #endif 

    return 0; 
}

static int create_ent_end_insert(piece_table* pt){

    pt_entry *curr_ent, *new_ent; 
    int new_ent_pos = new_pt_entry(pt);

    new_ent = &(pt->entries[new_ent_pos]);
    curr_ent = &(pt->entries[pt->curr_ent_ptr]);

    int old_left = curr_ent->left; 

    curr_ent->left = new_ent_pos; 
    new_ent->left = old_left; 
    new_ent->right = pt->curr_ent_ptr;

    pt->curr_ins_ent = new_ent_pos; 

  
    #ifdef DEBUG_INSERT
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Infront Insert]: ent pos: %d\n", new_ent_pos);
        log_to_file(&sk_logger, pbuf);
    #endif 
    
    return 0; 
}

static int create_middle_insert(piece_table* pt){

    // get info needed to be saved 

    int old_ent_ptr = pt->curr_ent_ptr; 
    size_t chr_ptr = pt->curr_chr_ptr;

    pt_entry* _ent = &(pt->entries[old_ent_ptr]);

    // push curr ent ptr to the undo stack 
    push_pt_stack(&(pt->undo), old_ent_ptr);

    size_t old_start = _ent->start; 
    size_t old_len = _ent->len; 
    pt_src_t old_src = _ent->src; 

    int old_left = _ent->left; 
    int old_right = _ent->right; 

    // create each of the new entries 
    int new_right_ent = new_pt_entry(pt);
    int new_midd_ent = new_pt_entry(pt); 
    int new_left_ent = new_pt_entry(pt);

    // adjust the fields for the new rightside entry 
    _ent = &(pt->entries[new_right_ent]);
    _ent->start = chr_ptr; 
    _ent->src = old_src; 

    size_t old_end = old_src + old_len; 
    size_t new_right_len = old_end - _ent->start; 

    _ent->len = new_right_len; 

    _ent->right = old_right; 
    _ent->left = new_midd_ent; 

    pt->curr_ent_ptr = new_right_ent;

    if (old_ent_ptr == pt->ent_tail)
        pt->ent_tail = new_right_ent;

    #ifdef DEBUG_INSERT
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Middle Insert - RIGHT] @ %d with start: %ld, len: %ld\n", 
            new_right_ent, _ent->start, _ent->len);
        log_to_file(&sk_logger, pbuf);
    #endif 

    // adjust the fields for the new leftside entry 
    _ent = &(pt->entries[new_left_ent]);

    _ent->start = old_start;  
    _ent->src = old_src; 
    _ent->len = old_len - new_right_len; 

    _ent->right = new_midd_ent; 
    _ent->left = old_left; 

    if (old_ent_ptr == pt->ent_head)
        pt->ent_head = new_left_ent; 

    #ifdef DEBUG_INSERT
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Middle Insert - LEFT] @ %d with start: %ld, len: %ld\n", 
            new_left_ent, _ent->start, _ent->len);
        log_to_file(&sk_logger, pbuf);
    #endif 

    // adjust fields for the new midd insertion entry 
    _ent = &(pt->entries[new_midd_ent]);
    _ent->right = new_right_ent; 
    _ent->left = new_left_ent;

    pt->curr_ins_ent = new_midd_ent; 

    #ifdef DEBUG_INSERT
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Middle Insert] @ %d with head: %d, tail: %d\n", 
            pt->curr_ins_ent, pt->ent_head, pt->ent_tail);
        log_to_file(&sk_logger, pbuf);
    #endif 

    // get the information for the currently pointd at entry 
    // pt_entry* curr_ent = CURR_ORG_ENT_PTR(pt);
    // int curr_ent_pos = ENT_POS_FROM_ORG_PTR(pt);
    // int curr_org_ptr = pt->curr_org_ptr;
    // size_t curs_pos =  pt->curr_chr_ptr; 

    // // add curr ptr at positon in org to undo stack 
    // push_pt_stack(&(pt->undo), curr_ent_pos);
    
    // // get the info from the curr_ent being split 
    // pt_entry* _ent; 
    // int shift_start, shift_end; 

    // pt_src_t orgn_ent_src = curr_ent->src; 
    // size_t orgn_end = (curr_ent->start + curr_ent->len);

    // int right = new_pt_insert_entry(pt);

    // _ent = ENT_AT_POS_ENTRIES(pt, right);
    // _ent->start = curs_pos; 
    // _ent->len =  orgn_end - _ent->start;
    // _ent->src = orgn_ent_src; 

    // int right_org_pos = pt->curr_org_ptr + 1; 

    // shift_start = pt->table.org_tail;
    // shift_end = right_org_pos; 

    // if (pt->table.org_tail == pt->curr_org_ptr){
    //     pt->table.org_tail = right_org_pos;
    // }else {
    //     shift_organizer_right(pt, shift_start, shift_end);
    //     pt->table.org_tail++; 
    // }
    
    // pt->table.organizer[right_org_pos] = right; 
    
    // size_t right_len = _ent->len; 

    

    // int left = new_pt_insert_entry(pt);

    // _ent = ENT_AT_POS_ENTRIES(pt, left);
    // _ent->start = curr_ent->start; 
    // _ent->len = curr_ent->len - right_len; 
    // _ent->src = orgn_ent_src;

    // int left_org_pos = curr_org_ptr - 1; 

    // shift_start = pt->table.org_head; 
    // shift_end = left_org_pos; 

    // if (pt->table.org_head == curr_org_ptr){
    //     pt->table.org_head = left_org_pos;
    // } else{
    //     shift_organizer_left(pt, shift_start, shift_end);
    //     pt->table.org_head--; 
    // }
        
    
    // pt->table.organizer[left_org_pos] = left;

    // int new_addition = new_pt_insert_entry(pt);
    // pt->curr_ins_org = pt->curr_org_ptr;
    // pt->table.organizer[pt->curr_ins_org] = new_addition; 
    // pt->curr_ins_ent = new_addition;
     
    // pt->curr_org_ptr = right_org_pos;
    

    #ifdef DEBUG_INSERT
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Middle Insert] with - org_pos: %d, head: %d, tail: %d\n", 
            pt->curr_ent_ptr, pt->ent_head, pt->ent_tail);
        log_to_file(&sk_logger, pbuf);
    #endif 

    return 0; 
}

int insert_manager(piece_table* pt, cursor_pos* curs_pos, char user_in){

    pt_entry *curr_ent, *curr_ins;
    // create the entries needed to handle the insert if they haven't been made already
    if ( pt->curr_ins_ent < 0 ){
        
        int ent_ptr = pt->curr_ent_ptr; 
        size_t chr_ptr = pt->curr_chr_ptr; 
        // curr_ent = CURR_ORG_ENT_PTR(pt);
        curr_ent = &(pt->entries[ent_ptr]);

        // if cursor is at the very end or beggining of the file
        int very_end = ent_ptr == pt->ent_tail && (curr_ent->start + curr_ent->len) == chr_ptr;
        int very_beginning = ent_ptr == pt->ent_head && curr_ent->start == chr_ptr;

        int at_ent_start = pt->curr_chr_ptr == curr_ent->start; 

        /* handle case where only ent is one with len == 0 */
        if ( (very_end || very_beginning) && curr_ent->len == 0){
            create_from_empty(pt);

        } else if ( very_end || very_beginning ){
            create_end_insert(pt, very_end);
        
        /* handle case where curr_chr_ptr is at the end of an existing entry */
        } else if (at_ent_start){
            create_ent_end_insert(pt);

        /* handle case where insert is in the middle of an existing entry */
        } else {
            create_middle_insert(pt);
        }   

    }

    // curr_ent = INSERT_ENT_PTR(pt);
    curr_ins = &(pt->entries[pt->curr_ins_ent]);

    add_buffer_t* adds = &(pt->addition);

    // check that there is space in additions
    check_additions_size(adds);

    adds->buf.text[adds->curr_pos] = user_in; 
    curr_ins->len++; 

    #ifdef DEBUG_INSERT
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, 
            "[INSERT {%c}] src: %s, ent_ins_ptr: %d, len: %ld, adds pos: %ld, chr_ptr: %ld\n", 
            adds->buf.text[adds->curr_pos],
            curr_ins->src == ORGN ? "ORG" : "ADD",
            pt->curr_ins_ent, 
            curr_ins->len, 
            adds->curr_pos, 
            pt->curr_chr_ptr);
        log_to_file(&sk_logger, pbuf);
    #endif 

    curs_pos->x++;
    adds->curr_pos++; 

    if (pt->ent_tail == pt->curr_ins_ent)
        pt->curr_chr_ptr++;
    
    #ifdef DEBUG_PT
        log_piece_table_current(&sk_logger, pt);
    #endif 

    // if (user_in == '\n'){

    // } else {
    //     curs_pos->x++;
    // }

    return 0; 
}