#include "pt_insert.h"

static int shift_organizer(piece_table* pt, int shift_start, int shift_end, int shift_dir){

    int shift = shift_dir * -1; 
    for (int i = shift_start; i < shift_end; i+= shift_dir){
        int ent_i = pt->table.organizer[i];
        pt->table.organizer[i + shift] = ent_i;
    }

    // update the head or tail depending on direction 
    if (shift_dir > 0)
        pt->table.org_tail += shift;  
    else 
        pt->table.org_head += shift; 
    
    return 0;
}

static int new_pt_insert_entry(piece_table* pt){
    // check if entries need to be reallocd 
    if (pt->table.ent_num + 1 == pt->table.ent_cap){

    }

    pt->table.ent_num++; 
    int new_ent_pos = pt->table.ent_num; 
    
    pt_entry* new_single_entry = &(pt->table.entries[new_ent_pos]);
    new_single_entry->src = ADD;
    new_single_entry->start = pt->addition.curr_pos; 
    new_single_entry->len = 0; 

    return new_ent_pos; 
}

static int create_end_insert(piece_table* pt, int very_end){
    
    int new_ent_pos = new_pt_insert_entry(pt);

    // place either at tail or the head 
    int pos = very_end ? pt->table.org_tail : pt->table.org_head;
    int direction = very_end ? pos + 1 : pos - 1; 
    
    // check if organizer needs to be reallocd first 
    if (pt->table.org_num + 1 == pt->table.org_cap){

    }

    // at the very end of the organizer, need to shift things over 
    // this is the worst case scenario for shifting 
    if (direction < 0 || direction == pt->table.org_cap){
        int shift_start = very_end ? pt->table.org_head : pt->table.org_tail;
        int shift_end = pos;
        int shift_dir = very_end ? -1 : 1; 
        int shift = shift_dir * -1; 

        for (int i = shift_start; i < shift_end; i += shift_dir){
            int ent_i = pt->table.organizer[i];
            pt->table.organizer[i + shift] = ent_i; 
        }

        // update the head or tail depending on direction 
        if (very_end)
            pt->table.org_head += shift;  
        else 
            pt->table.org_tail += shift; 
    
    } 

    // update head or tail depending on direction
    pt->table.organizer[direction] = new_ent_pos;  
    pt->curr_org_ptr = direction;

    memset(pbuf, 0, PBUF_SIZE);
    sprintf(pbuf, "ent pos: %d, dir: %d\n", new_ent_pos, direction);
    log_to_file(&sk_logger, pbuf);

    if (very_end) 
        pt->table.org_tail = direction;
    else 
        pt->table.org_head = direction;

    return 0; 
}

static int create_ent_end_insert(piece_table* pt){
    int new_ent_pos = new_pt_insert_entry(pt);

    // check if organizer needs to be reallocd and re-organized 
    if (pt->table.org_num + 1 == pt->table.org_cap){

    }

    int curr_ent = pt->curr_org_ptr;

    int lefthand_num = curr_ent - pt->table.org_head; 
    int righthand_num = curr_ent - pt->table.org_tail;

    // shift whichever side has less entries 
    int shift_cond = lefthand_num < righthand_num;
    int shift_start = shift_cond ? pt->table.org_head : pt->table.org_tail; 
    int shift_end = shift_cond ? curr_ent : curr_ent + 1;  
    int shift_dir = shift_cond ? -1 : 1;

    // do the shifting here in a function call

    // if shifting to the right happens 
    if (!shift_cond)
        pt->curr_org_ptr++;  
    
    pt->table.organizer[pt->curr_org_ptr] = new_ent_pos;
    
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