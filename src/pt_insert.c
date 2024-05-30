#include "pt_insert.h"

static int shift_organizer_right(piece_table* pt, int start, int end){

    memset(pbuf, 0, PBUF_SIZE);
    sprintf(pbuf, "[Shift Right Details]: start: %d, end: %d\n",start, end);
    log_to_file(&sk_logger, pbuf);

    // check that a shift does not go outside of the bounds of the organizer
    // by making sure the organizer is big enough 
    check_organizer_size(pt);

    for (int i = start; i >= end; i--){
        int ent_i = pt->table.organizer[i];
        pt->table.organizer[i + 1] = ent_i;
    }

    pt->table.org_tail++; 

    return 0; 
 
}

static int shift_organizer_left(piece_table* pt, int start, int end){
    
    memset(pbuf, 0, PBUF_SIZE);
    sprintf(pbuf, "[Shift Left Details]: start: %d, end: %d\n",start, end);
    log_to_file(&sk_logger, pbuf);

    // check that a shift does not go outside of the bounds of the organizer

    for (int i = start; i <= end; i++){
        int ent_i = pt->table.organizer[i];
        pt->table.organizer[i - 1] = ent_i;
    }

    pt->table.org_head--; 

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

    // check the organizer 
    check_organizer_size(pt);

    // update head or tail depending on next_pos
    pt->table.organizer[next_pos] = new_ent_pos; 
    pt->curr_ins_org = new_ent_pos;

    if (very_end){
        pt->curr_chr_ptr = pt->addition.curr_pos - 1; // point to last char added
        pt->curr_org_ptr = next_pos; 
    }
        

    if (very_end) 
        pt->table.org_tail = next_pos;
    else 
        pt->table.org_head = next_pos;

    memset(pbuf, 0, PBUF_SIZE);
    sprintf(pbuf, "[Ends insert]: ent pos: %d, org_new_pos: %d\n", new_ent_pos, next_pos);
    log_to_file(&sk_logger, pbuf);

    return 0; 
}

static int create_ent_end_insert(piece_table* pt){
    int new_ent_pos = new_pt_insert_entry(pt);

    // check if organizer needs to be reallocd and re-organized 
    check_organizer_size(pt);

    int curr_org_ent = pt->curr_org_ptr;

    int lefthand_num = curr_org_ent - pt->table.org_head; 
    int righthand_num = curr_org_ent - pt->table.org_tail;

    // shift whichever side has less entries 
    int going_left = lefthand_num < righthand_num;
    int shift_start = going_left ? pt->table.org_head : pt->table.org_tail; 
    int shift_end = going_left ? curr_org_ent - 1 : curr_org_ent;  

    if (going_left){
        shift_organizer_left(pt, shift_start, shift_end);
        pt->table.organizer[pt->curr_org_ptr - 1] = new_ent_pos;
        pt->curr_ins_org = new_ent_pos;
        // pt->curr_org_ptr = curr_org_ent;     // stays the same 
    } else{ 
        shift_organizer_right(pt, shift_start, shift_end);
        pt->table.organizer[curr_org_ent] = new_ent_pos;
        pt->curr_ins_org = new_ent_pos;
        pt->curr_org_ptr = curr_org_ent + 1; 
    }
  
    memset(pbuf, 0, PBUF_SIZE);
    sprintf(pbuf, "[Infront Insert]: ent pos: %d, dir: %d\n", new_ent_pos, pt->curr_org_ptr);
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
    int shift_start, shift_end; 

    pt_src_t orgn_ent_src = curr_ent->src; 
    size_t orgn_end = (curr_ent->start + curr_ent->len);

    int right = new_pt_insert_entry(pt);

    _ent = ENT_AT_POS_ENTRIES(pt, right);
    _ent->start = curs_pos; 
    _ent->len =  orgn_end - _ent->start;
    _ent->src = orgn_ent_src; 

    int right_org_pos = pt->curr_org_ptr + 1; 

    shift_start = pt->table.org_tail;
    shift_end = right_org_pos; 

    if (pt->table.org_tail == pt->curr_org_ptr)
        pt->table.org_tail = right_org_pos;
    
    else
        shift_organizer_right(pt, shift_start, shift_end);
    
    pt->table.organizer[right_org_pos] = right; 
    
    size_t right_len = _ent->len; 

    memset(pbuf, 0, PBUF_SIZE);
    sprintf(pbuf, "[Middle Insert - RIGHT] with start: %ld, len: %ld, org pos: %d, ent num: %d\n", 
        _ent->start, _ent->len, right_org_pos, right);
    log_to_file(&sk_logger, pbuf);

    int left = new_pt_insert_entry(pt);

    _ent = ENT_AT_POS_ENTRIES(pt, left);
    _ent->start = curr_ent->start; 
    _ent->len = curr_ent->len - right_len; 
    _ent->src = orgn_ent_src;

    int left_org_pos = curr_org_ptr - 1; 

    shift_start = pt->table.org_head; 
    shift_end = left_org_pos; 

    if (pt->table.org_head == curr_org_ptr)
        pt->table.org_head = left_org_pos;
    
    else
        shift_organizer_left(pt, shift_start, shift_end);
    
    pt->table.organizer[left_org_pos] = left;

    memset(pbuf, 0, PBUF_SIZE);
    sprintf(pbuf, "[Middle Insert - LEFT] with start: %ld, len: %ld, org pos: %d, ent num: %d\n", 
        _ent->start, _ent->len, left_org_pos, left);
    log_to_file(&sk_logger, pbuf);

    int new_addition = new_pt_insert_entry(pt);
    pt->table.organizer[pt->curr_org_ptr] = new_addition; 
    pt->curr_ins_org = new_addition;

    pt->curr_org_ptr = right_org_pos;
    

    memset(pbuf, 0, PBUF_SIZE);
    sprintf(pbuf, "[Middle Insert] with - org_pos: %d, head: %d, tail: %d\n", 
        pt->curr_org_ptr, pt->table.org_head, pt->table.org_tail);
    log_to_file(&sk_logger, pbuf);

    
     
    return 0; 
}

int insert_manager(piece_table* pt, cursor_pos* curs_pos, char user_in){

    pt_entry* curr_ent;
    // create the entries needed to handle the insert if they haven't been made already
    if ( pt->curr_ins_org < 0 ){
        
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

    curr_ent = INSERT_ENT_PTR(pt);

    add_buffer_t* adds = &(pt->addition);

    // check that there is space in additions
    check_additions_size(adds);

    adds->buf.text[adds->curr_pos] = user_in; 
    curr_ent->len++; 

    memset(pbuf, 0, PBUF_SIZE);
    sprintf(pbuf, 
        "[INSERT {%c}] src: %s, ent_ins_ptr: %d, len: %ld, adds pos: %ld, chr_ptr: %ld\n", 
        adds->buf.text[adds->curr_pos],
        curr_ent->src == ORGN ? "ORG" : "ADD",
        pt->curr_ins_org, 
        curr_ent->len, 
        adds->curr_pos, 
        pt->curr_chr_ptr);
    log_to_file(&sk_logger, pbuf);

    curs_pos->x++;
    adds->curr_pos++; 

    if (pt->table.organizer[pt->table.org_tail] == pt->curr_ins_org)
        pt->curr_chr_ptr++;
    
    log_piece_table_current(&sk_logger, pt);

    // if (user_in == '\n'){

    // } else {
    //     curs_pos->x++;
    // }

    return 0; 
}