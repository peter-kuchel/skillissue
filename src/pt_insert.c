#include "pt_insert.h"

static int create_from_empty(piece_table* pt){

    int new_ent_pos = new_pt_entry(pt);

    push_pt_stack(&(pt->undo), pt->curr_ent_ptr);

    pt->curr_ent_ptr = new_ent_pos;
    pt->curr_ins_ent = new_ent_pos; 

    pt->ent_tail = new_ent_pos;
    pt->ent_head = new_ent_pos;
    

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

    pt_entry *curr_ent, *new_ent, *old_left_ent; 
    int new_ent_pos = new_pt_entry(pt);

    new_ent = &(pt->entries[new_ent_pos]);
    curr_ent = &(pt->entries[pt->curr_ent_ptr]);

    int old_left = curr_ent->left; 

    old_left_ent = &(pt->entries[old_left]);

    curr_ent->left = new_ent_pos; 
    new_ent->left = old_left; 
    new_ent->right = pt->curr_ent_ptr;

    old_left_ent->right = new_ent_pos;

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
    pt_entry *_ent, *_side_ents; 

    int old_ent_ptr = pt->curr_ent_ptr; 
    size_t chr_ptr = pt->curr_chr_ptr;

    _ent = &(pt->entries[old_ent_ptr]);

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

    size_t old_end = old_start + old_len; 
    size_t new_right_len = old_end - _ent->start; 

    _ent->len = new_right_len; 

    _ent->right = old_right; 
    _ent->left = new_midd_ent; 

    pt->curr_ent_ptr = new_right_ent;

    if (old_ent_ptr == pt->ent_tail)
        pt->ent_tail = new_right_ent;
    else {
        _side_ents = &(pt->entries[old_right]);
        _side_ents->left = new_right_ent; 
    }

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
    else{
        _side_ents = &(pt->entries[old_left]);
        _side_ents->right = new_left_ent; 
    }

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

    return 0; 
}

static int handle_new_line_insert(piece_table* pt, cursor_pos* curs_pos){

    // current line won't be updated here 
    int new_line = add_new_line(&(pt->lh));

    line *_new_line, *curr_line;  

    if (pt->curr_ins_ent == pt->ent_tail){
        _new_line = &(pt->lh.lines[new_line]);
        _new_line->line_size = 0; 

        pt->lh.curr_line = new_line;

        curs_pos->x = 0; 
        curs_pos->y++;

        #ifdef DEBUG_INSERT
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, "[New Line Insert at tail]");
            log_to_file(&sk_logger, pbuf);
        #endif

        return 0; 
    }

    // calc new line sizes with the new addition 
    // int move_ent = pt->curr_ins_ent;
    int move_ent = (pt->entries[pt->curr_ins_ent]).right; 
    size_t chr_ptr;
    pt_entry* _ent; 
    pt_buffer_t* src_buff; 

    int parsing = 1; 
    int right_dir_size = 0;

    _ent = &(pt->entries[move_ent]);
    // chr_ptr = pt->curr_chr_ptr; 
    chr_ptr = _ent->start;
    

    #ifdef DEBUG_INSERT
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[current move_ent]: %d, size: %ld\n", move_ent, _ent->len);
        log_to_file(&sk_logger, pbuf);
    #endif 
   
    while (parsing){

        src_buff = GET_PT_BUFF(pt, _ent->src);
        
        do {

            #ifdef DEBUG_INSERT
                char _c = PTR_AT_CHR(pt, chr_ptr);
                memset(pbuf, 0, PBUF_SIZE);
                sprintf(pbuf, "[%c] ", 
                    _c);
                log_to_file(&sk_logger, pbuf);
            #endif 

            if (src_buff->text[chr_ptr] == '\n'){

                parsing--;

                #ifdef DEBUG_INSERT
                    memset(pbuf, 0, PBUF_SIZE);
                    sprintf(pbuf, "\n[FINISHED PARSING]\n");
                    log_to_file(&sk_logger, pbuf);
                #endif  
                break; 
            }

            chr_ptr++;
            right_dir_size++;
        } while (chr_ptr < (_ent->len + _ent->start));
        
        // update to the next ent
        if (parsing){
            move_ent = _ent->right;
            #ifdef DEBUG_INSERT
                memset(pbuf, 0, PBUF_SIZE);
                sprintf(pbuf, "\n[next move_ent in newline]: %d\n", move_ent);
                log_to_file(&sk_logger, pbuf);
            #endif 

            if (move_ent == NULL_ENT){
                parsing--;
            } else {
                _ent = &(pt->entries[move_ent]);
                chr_ptr = _ent->start;
            }
        }
    }; 

    // get current line 
    _new_line = &(pt->lh.lines[new_line]);
    curr_line = LH_CURR_LINE(pt);
    
    curr_line->line_size = curr_line->line_size - right_dir_size;
    _new_line->line_size = right_dir_size;

    pt->lh.curr_line = new_line;

    curs_pos->x = 0; 
    curs_pos->y++; 

    #ifdef DEBUG_INSERT
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[New Line Insert]: {%d} , size = %d\n", 
            new_line, _new_line->line_size);
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

    
    adds->curr_pos++; 

    if (pt->ent_tail == pt->curr_ins_ent)
        pt->curr_chr_ptr++;

    // handle when a new line is added 
    if (user_in == '\n'){
        handle_new_line_insert(pt, curs_pos);
    } else {
        
        line* curr_line = LH_CURR_LINE(pt);
        curr_line->line_size++;
        curs_pos->x++;
    }
    
    #ifdef DEBUG_PT
        log_piece_table_current(&sk_logger, pt);
    #endif 


    return 0; 
}