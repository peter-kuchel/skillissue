#include "pt_insert.h"

static int shift_organizer(piece_table* pt, int shift_start, int shift_end, int shift_dir){

    // check that a shift in this direction is possible - not already at an edge 
    memset(pbuf, 0, PBUF_SIZE);
    sprintf(pbuf, "[Shift Details]: start: %d, end: %d, dir: %d \n", 
                shift_start, shift_end, shift_dir);
    log_to_file(&sk_logger, pbuf);

    int inc_ent = shift_dir * -1; 
    for (int i = shift_end; i >= shift_start; i += inc_ent){
        int ent_i = pt->table.organizer[i];
        pt->table.organizer[i + shift_dir] = ent_i;
    }

    // // update the head or tail depending on next_pos 
    // if (pt->table.org_head == pt->table.org_tail) return 0; 

    if (inc_ent < 0)
        pt->table.org_tail += shift_dir;  
    else 
        pt->table.org_head += shift_dir; 
    
    return 0;
}

static int new_pt_insert_entry(piece_table* pt){
    // check if entries need to be reallocd 
    check_entries_size(pt);

    pt->table.ent_num++; 
    int new_ent_pos = pt->table.ent_num; 
    
    pt_entry* new_single_entry = ENT_AT_POS_ENTRIES(pt, new_ent_pos);
    new_single_entry->src = ADD;
    new_single_entry->start = pt->addition.curr_pos; 
    new_single_entry->len = 0; 

    return new_ent_pos; 
}

static int create_end_insert(piece_table* pt, int very_end){
    
    int new_ent_pos = new_pt_insert_entry(pt);

    // place either at tail or the head 
    int pos = very_end ? pt->table.org_tail : pt->table.org_head;
    int next_pos = very_end ? pos + 1 : pos - 1; 
    
    // check if organizer needs to be reallocd first 
    if (pt->table.org_num + 1 == pt->table.org_cap){

    }

    // at the very end of the organizer, need to shift things over 
    // this is the worst case scenario for shifting 
    if (next_pos < 0 || next_pos == pt->table.org_cap){
        int shift_start = very_end ? pt->table.org_head : pt->table.org_tail;
        int shift_end = pos;
        int shift_dir = very_end ? -1 : 1; 

        shift_organizer(pt, shift_start, shift_end, shift_dir);
    
    } 

    // update head or tail depending on next_pos
    pt->table.organizer[next_pos] = new_ent_pos;  
    pt->curr_org_ptr = next_pos;

    memset(pbuf, 0, PBUF_SIZE);
    sprintf(pbuf, "ent pos: %d, org_new_pos: %d\n", new_ent_pos, next_pos);
    log_to_file(&sk_logger, pbuf);

    if (very_end) 
        pt->table.org_tail = next_pos;
    else 
        pt->table.org_head = next_pos;

    return 0; 
}

static int create_ent_end_insert(piece_table* pt){
    int new_ent_pos = new_pt_insert_entry(pt);

    // check if organizer needs to be reallocd and re-organized 
    check_organizer_size(pt);

    int curr_ent = pt->curr_org_ptr;

    int lefthand_num = curr_ent - pt->table.org_head; 
    int righthand_num = curr_ent - pt->table.org_tail;

    // shift whichever side has less entries 
    int going_left = lefthand_num < righthand_num;
    int shift_start = going_left ? pt->table.org_head : pt->table.org_tail; 
    int shift_end = going_left ? curr_ent : curr_ent + 1;  
    int shift_dir = going_left ? -1 : 1;

    // do the shifting here in a function call
    shift_organizer(pt, shift_start, shift_end, shift_dir);

    // if shifting to the right happens 
    if (!going_left)
        pt->curr_org_ptr++;  
    
    pt->table.organizer[pt->curr_org_ptr] = new_ent_pos;

    memset(pbuf, 0, PBUF_SIZE);
    sprintf(pbuf, "ent pos: %d, dir: %d\n", new_ent_pos, pt->curr_org_ptr);
    log_to_file(&sk_logger, pbuf);
    
    return 0; 
}

static int create_middle_insert(piece_table* pt){

    // get the information for the currently pointd at entry 
    pt_entry* curr_ent = CURR_ORG_ENT_PTR(pt);
    int curr_ent_pos = ENT_POS_FROM_ORG_PTR(pt);
    int curr_org_ptr = pt->curr_org_ptr;
    size_t curs_pos =  pt->curr_chr_ptr; 

    // add curr ptr at positon in org to undo stack 
    push_pt_stack(&(pt->undo), curr_ent_pos);
    
    // get the info from the curr_ent being split 
    pt_entry* _ent; 
    int shift_start, shift_end, shift_dir; 

    pt_src_t orgn_ent_src = curr_ent->src; 
    size_t orgn_end = curr_ent->start + curr_ent->len;

    int right = new_pt_insert_entry(pt);

    _ent = ENT_AT_POS_ENTRIES(pt, right);
    _ent->start = curs_pos + 1; 
    _ent->len =  orgn_end - _ent->start;
    _ent->src = orgn_ent_src; 

    int right_org_pos = pt->curr_org_ptr + 1; 

    shift_start = pt->table.org_tail;
    shift_end = right_org_pos; 
    shift_dir = 1; 

    
    if (pt->table.org_tail == pt->curr_org_ptr)
        pt->table.org_tail = right_org_pos;
    
    else
        shift_organizer(pt, shift_start, shift_end, shift_dir);
    

    pt->table.organizer[right_org_pos] = right; 
    
    size_t right_len = _ent->len; 

    log_piece_table_current(&sk_logger, pt);

    int left = new_pt_insert_entry(pt);

    _ent = ENT_AT_POS_ENTRIES(pt, left);
    _ent->start = curr_ent->start; 
    _ent->len = orgn_end - right_len; 
    _ent->src = orgn_ent_src;

    int left_org_pos = curr_org_ptr - 1; 

    shift_start = pt->table.org_head; 
    shift_end = left_org_pos; 
    shift_dir = -1; 

    
    if (pt->table.org_head == curr_org_ptr)
        pt->table.org_head = left_org_pos;
    
    else
        shift_organizer(pt, shift_start, shift_end, shift_dir);
    
    pt->table.organizer[left_org_pos] = left;

    log_piece_table_current(&sk_logger, pt);

    int new_addition = new_pt_insert_entry(pt);
    pt->table.organizer[pt->curr_org_ptr] = new_addition; 

    memset(pbuf, 0, PBUF_SIZE);
    sprintf(pbuf, "after insertion: org_pos: %d, head: %d, tail: %d\n", 
        pt->curr_org_ptr, pt->table.org_head, pt->table.org_tail);
    log_to_file(&sk_logger, pbuf);
     
    return 0; 
}

int insert_manager(piece_table* pt, cursor_pos* curs_pos, char user_in){

    pt_entry* curr_ent;
    // create the entries needed to handle the insert 
    if ( !pt->insert_ready ){
        
        int ent_ptr = pt->curr_org_ptr; 
        size_t chr_ptr = pt->curr_chr_ptr; 
        curr_ent = CURR_ORG_ENT_PTR(pt);

        // if cursor is at the very end or beggining of the file
        int very_end = ent_ptr == pt->table.org_tail && (curr_ent->start + curr_ent->len) == chr_ptr;
        int very_beginning = ent_ptr == pt->table.org_head && curr_ent->start == chr_ptr;

        int at_ent_start = pt->curr_chr_ptr == curr_ent->start; 

        if ( very_end || very_beginning ){
            create_end_insert(pt, very_end);
        
        /* handle case where curr_chr_ptr is at the end of an existing entry */
        } else if (at_ent_start){
            create_ent_end_insert(pt);

        /* handle case where insert is in the middle of an existing entry */
        } else {
            create_middle_insert(pt);
        }   

        pt->insert_ready++;
    }

    curr_ent = CURR_ORG_ENT_PTR(pt);

        
    add_buffer_t* adds = &(pt->addition);

    // check that there is space in additions
    if (adds->curr_pos + 1 == adds->buf.size){

    }

    adds->buf.text[adds->curr_pos] = user_in; 
    curr_ent->len++; 

    memset(pbuf, 0, PBUF_SIZE);
    sprintf(pbuf, 
        "[INSERT {%c}] ent_org_ptr: %d, len: %ld, curr_pos: %ld, src: %d\n", adds->buf.text[adds->curr_pos],
        pt->curr_org_ptr, curr_ent->len, adds->curr_pos, curr_ent->src);
    log_to_file(&sk_logger, pbuf);

    adds->curr_pos++; 
    pt->curr_chr_ptr = adds->curr_pos;

    if (user_in == '\n'){

    } else {
        curs_pos->x++;
    }

    return 0; 
}