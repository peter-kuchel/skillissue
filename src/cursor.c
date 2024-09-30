#include "cursor.h"

// TODO: all this code should be refactored because its ugly af 


// for when a or d is pressed 
int handle_side_movement(piece_table* pt, cursor_pos* pos, int dir, line_view* lv){
    
    // reset the column memory when moving across the line
    pt->lh.col_mem = -1; 

    pt_entry* ent = &(pt->entries[pt->curr_ent_ptr]);
    size_t chr_ptr = pt->curr_chr_ptr; 

    /* move to the right -- when d is pressed*/  
    if (dir > 0){
        size_t upper_bound = ent->start + ent->len;

        // go forward to next entry, 
        if (chr_ptr == upper_bound || chr_ptr + 1 == upper_bound){
            
            #ifdef DEBUG_MOVE
                memset(pbuf, 0, PBUF_SIZE);
                sprintf(pbuf, "[At (or past) upper bound]\n");
                log_to_file(&sk_logger, pbuf);
            #endif 

            // currently in the last entry 
            if (pt->ent_tail == pt->curr_ent_ptr){
                
                #ifdef DEBUG_MOVE
                    memset(pbuf, 0, PBUF_SIZE);
                    sprintf(pbuf, "[At far left entry]: chr_ptr = %ld\n",chr_ptr);
                    log_to_file(&sk_logger, pbuf);
                #endif 

                // if at the very end of the file 
                if (chr_ptr + 1 == upper_bound){
                    pos->x++;
                    pt->curr_chr_ptr++; 

                    #ifdef DEBUG_MOVE
                        memset(pbuf, 0, PBUF_SIZE);
                        sprintf(pbuf, "[currently at very end] before: %ld, after: %ld\n", 
                            chr_ptr, pt->curr_chr_ptr);
                        log_to_file(&sk_logger, pbuf);
                    #endif 
                }

                return 0;
            }

            // if we are at the every end of the current entry -- move into the right entry
            if (chr_ptr + 1 == upper_bound){
                
                int right = (&(pt->entries[pt->curr_ent_ptr]))->right;

                ent = &(pt->entries[right]);

                pt->curr_chr_ptr = ent->start;
                pt->curr_ent_ptr = right;

                #ifdef DEBUG_MOVE
                    memset(pbuf, 0, PBUF_SIZE);
                    sprintf(pbuf, 
                    "[going into next entry + 1]: curr_org_ptr: %d, curr_chr_ptr before: %ld, after: %ld\n", 
                        pt->curr_ent_ptr, chr_ptr, pt->curr_chr_ptr);
                    log_to_file(&sk_logger, pbuf);
                #endif 
            } 
        } else {
            pt->curr_chr_ptr++;
        }


        #ifdef DEBUG_MOVE
            char _c = PTR_AT_CHR(pt, pt->curr_chr_ptr);
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(
                pbuf, 
                "[RIGHT MOVE] chr_ptr before: %ld, after: %ld | %c | ub: %ld| org_pos: %d, org_tail: %d\n", 
                chr_ptr, pt->curr_chr_ptr, _c, 
                upper_bound, pt->curr_ent_ptr, pt->ent_tail);
            log_to_file(&sk_logger, pbuf);
        #endif 

        pos->x++;

        update_view_move_right(lv, pos);
        

    /* move to the left -- when a is pressed*/ 
    } else {
        size_t lower_bound = ent->start; 

        if (chr_ptr == lower_bound || chr_ptr - 1 == lower_bound){
            #ifdef DEBUG_MOVE
                memset(pbuf, 0, PBUF_SIZE);
                sprintf(pbuf, "[At (or past) lower bound]\n");
                log_to_file(&sk_logger, pbuf); 
            #endif 
            
            if (pt->ent_head == pt->curr_ent_ptr){

                #ifdef DEBUG_MOVE
                    memset(pbuf, 0, PBUF_SIZE);
                    sprintf(pbuf, "[At far left entry]: chr_ptr = %ld\n",chr_ptr);
                    log_to_file(&sk_logger, pbuf);
                #endif  

                // at the very beginning of the file
                if (chr_ptr - 1 == lower_bound){
                    
                    pos->x--;
                    pt->curr_chr_ptr--; 

                    #ifdef DEBUG_MOVE
                        memset(pbuf, 0, PBUF_SIZE);
                        sprintf(pbuf, "[currently at very end] before: %ld, after: %ld\n",
                            chr_ptr, pt->curr_chr_ptr);
                        log_to_file(&sk_logger, pbuf);
                    #endif 
                }

                // so it doesn't return before handling shifting window left 
                update_view_move_left(lv, pos);
                return 0;
            }
            
            // if we are at the every end of the current entry -- move into the right entry
            if (chr_ptr == lower_bound){

                int left = (&(pt->entries[pt->curr_ent_ptr]))->left;

                ent = &(pt->entries[left]);

                pt->curr_chr_ptr = (ent->start + ent->len); 
                pt->curr_ent_ptr = left;

                #ifdef DEBUG_MOVE
                    memset(pbuf, 0, PBUF_SIZE);
                    sprintf(pbuf, 
                        "[going into next entry - 1]: curr_org_ptr: %d, curr_chr_ptr before: %ld, after: %ld\n", 
                        pt->curr_ent_ptr, chr_ptr, pt->curr_chr_ptr);
                    log_to_file(&sk_logger, pbuf);
                #endif 
            }
        }

        pos->x--;
        pt->curr_chr_ptr--; 

        #ifdef DEBUG_MOVE
            char _c = PTR_AT_CHR(pt, pt->curr_chr_ptr);
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(
                pbuf, 
                "[LEFT MOVE] chr_ptr before: %ld, after: %ld| %c | lb: %ld| org_pos: %d, org_head: %d\n", 
                chr_ptr, pt->curr_chr_ptr, _c, 
                lower_bound, pt->curr_ent_ptr, pt->ent_head);
            log_to_file(&sk_logger, pbuf);
        #endif 

        update_view_move_left(lv, pos);

    }

    return 0; 
}

int handle_jump_down(piece_table* pt, cursor_pos* pos, int prev_chr_ptr, int prev_ent, line_view* lv){

    // get the character last seen when attempting to jump down
    pt_buffer_t *src_buffer = GET_PT_BUFF( pt, (pt->entries[prev_ent]).src );
    char prev_char = src_buffer->text[prev_chr_ptr]; 

    // #ifdef DEBUG_MOVE
    //     memset(pbuf, 0, PBUF_SIZE);
    //     sprintf(pbuf, 
    //         "[JUMP TEST DOWN]: %x\n", prev_char); 
    //     log_to_file(&sk_logger, pbuf);
    // #endif 

    line_handler* lh = LH_PTR(pt);

    // jump down only if we are not already at the bottom else don't 
    if ( prev_char == '\n' && lh->curr_line != lh->bottom_line){

        // int prev_curr_line = lh->curr_line; 
        line *curr = LH_CURR_LINE(pt);
        lh->curr_line = curr->next_line; 
        lh->line_number++; 

        int term_rh_end = lv->tinfo_ptr->cols; 
        if (lv->right_win > term_rh_end){
            lv->right_win = term_rh_end;
            lv->left_win = 0; 
        }

        pos->y++;
        pos->x = 0;

        // handle if jump down goes into line off of current view of the screen 
        // update_view_move_down(pt, lv, pos);
        
        #ifdef DEBUG_SCREEN
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, 
                "[JUMP DOWN]: curr line is: %d\n", lh->curr_line);
            log_to_file(&sk_logger, pbuf);
        #endif 
    }

    return 0; 
}

int handle_jump_up(piece_table* pt, cursor_pos* pos, line_view* lv){

    char _c = PTR_AT_CHR(pt, pt->curr_chr_ptr);

    #ifdef DEBUG_MOVE
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, 
            "[JUMP TEST UP]: %x\n", _c);
        log_to_file(&sk_logger, pbuf);
    #endif 

    // jump up if we hit a new line character 
    if ( _c == '\n'){
        
        line *curr = LH_CURR_LINE(pt);
        pt->lh.curr_line = curr->prev_line; 
        pt->lh.line_number--; 

        int curr_l_size = CURR_LINE_SIZE(pt);

        pos->y--; 
        pos->x = curr_l_size;

        int term_rh_end = lv->tinfo_ptr->cols;
        if (pos->x > term_rh_end){

            lv->right_win = pos->x + 1; 
            lv->left_win = pos->x - term_rh_end; 
        }

        // handle if jump up goes into line off of current view of the screen
        // if (curr_l_size > lv) 
        // update_view_move_up(pt, lv, pos);

        #ifdef DEBUG_MOVE
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, 
                "[JUMP UP]: curr line is: %d\n", pt->lh.curr_line);
            log_to_file(&sk_logger, pbuf);
        #endif
        
    }
    return 0; 
}

// move the chr_ptr to the correct place when going up and down the file
static int move_chr_ptr(piece_table* pt, int dist, int dir){

    int curr_ent = pt->curr_ent_ptr; 
    pt_entry *curr = &(pt->entries[curr_ent]);

    // depending on the direction (up or down), the entry end we want to look for will be different
    size_t ent_end = dir > 0 ? curr->start + (curr->len - 1) : curr->start; 

    // at last entry and at the every end of it
    if ( curr_ent == pt->ent_tail && 
         pt->curr_chr_ptr == curr->start + (curr->len - 1) 
    ){
        pt->curr_chr_ptr--;
    }

    int d = 0; 
    while (d <= dist){

        #ifdef DEBUG_MOVE
            char _c = PTR_AT_CHR(pt,pt->curr_chr_ptr);
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, 
                "[dir: %d] %d <= %d (dist) | at: {%ld} -> end {%ld}| curr_ent: %d [%c]\n",
                dir, d, dist, pt->curr_chr_ptr, ent_end, curr_ent, _c  
            );
            log_to_file(&sk_logger, pbuf);

            // if(_c == '\t'){
            //     memset(pbuf, 0, PBUF_SIZE);
            //     sprintf(pbuf,"[c is a tab: \\t]\n");
            //     log_to_file(&sk_logger, pbuf);
            // }
        #endif 

        if (pt->curr_chr_ptr == ent_end){

            #ifdef DEBUG_MOVE
                    memset(pbuf, 0, PBUF_SIZE);
                    sprintf(pbuf, 
                        "[REACH END of current ENT]\n");
                    log_to_file(&sk_logger, pbuf);
                #endif 

            // check if moving down will hit the end of the file
            if (pt->ent_tail == curr_ent && dir == 1){
                pt->curr_chr_ptr++;

            // else go into the next entry 
            } else {
                curr_ent = dir < 0 ? curr->left : curr->right; 
                curr = &(pt->entries[curr_ent]);
                
                pt->curr_chr_ptr = dir > 0 ? curr->start : curr->start + (curr->len - 1); 
                pt->curr_ent_ptr = curr_ent;
                ent_end = dir > 0 ? curr->start + (curr->len - 1) : curr->start;

                #ifdef DEBUG_MOVE
                    memset(pbuf, 0, PBUF_SIZE);
                    sprintf(pbuf, 
                        "[REACH END]: new-- end: %ld, curr_ent: %d, curr_chr_ptr: %ld\n",
                            ent_end, curr_ent, pt->curr_chr_ptr
                    );
                    log_to_file(&sk_logger, pbuf);
                #endif 
            }

        } else{

            if (dir > 0) 
                pt->curr_chr_ptr++;
            else         
                pt->curr_chr_ptr--; 
        }

        d++; 
    }

    return 0; 
}


// for when w or s is pressed 
int handle_line_movement(piece_table* pt, cursor_pos* pos, int dir, line_view* lv){
    line *curr, *jump_to;
    int dist, jump_size, curr_size, curr_col_mem;

    // save the current column for the movement 
    if (pt->lh.col_mem < 0)
        pt->lh.col_mem = pos->x; 
    
    curr_col_mem = pt->lh.col_mem;
  
    // get current line and it's size 
    int _curr_line = pt->lh.curr_line; 
    curr = &(pt->lh.lines[_curr_line]);
    curr_size = curr->line_size;

    // if going up the file by pressing 'w'
    if (dir < 0){

        if (pt->lh.top_line == _curr_line) return 0; 

        pos->y--; 

        // the line that is going to be jumped to  & size of the above line to calcuate where the char ptr will end up 
        jump_to = &(pt->lh.lines[curr->prev_line]);
        jump_size = jump_to->line_size;

        // check if the size of the line is greater than the column memery that the cursor should jump to
        if (jump_size >= curr_col_mem){

            dist = pos->x + (jump_size - curr_col_mem);             // distance to move chr_ptr
            pos->x = curr_col_mem;
        
        } else {

            dist = pos->x;
            pos->x = jump_size;
        } 
        
        move_chr_ptr(pt, dist, dir);
        pt->lh.curr_line = curr->prev_line;
        pt->lh.line_number--;

        #ifdef DEBUG_MOVE
            char _c = PTR_AT_CHR(pt,pt->curr_chr_ptr);
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, 
                "[moving up 1 with col mem: (%d)]: xpos: %d, curr: %d, chr_ptr: %ld, dist: %d, line size: %d | [%c],\n",
                curr_col_mem, pos->x, pt->lh.curr_line, pt->curr_chr_ptr, dist, jump_to->line_size, _c
                );
            log_to_file(&sk_logger, pbuf);
        #endif

        update_view_move_up(pt, lv, pos);

    // else going down the file pressing 's' 
    } else {

        // if current line is bottom line, update window if necessary 
        if (pt->lh.bottom_line == _curr_line){
            // update_view_move_down(pt, lv, pos);
            return 0; 
        }

        pos->y++; 

        jump_to = &(pt->lh.lines[curr->next_line]);
        jump_size = jump_to->line_size;

        if (jump_size >= curr_col_mem){
            dist = curr_col_mem + (curr_size - pos->x);         // distance to move chr_ptr
            pos->x = curr_col_mem;
        } 
        
        else{
            dist = jump_size + (curr_size - pos->x); 
            pos->x = jump_size;
        }
        

        move_chr_ptr(pt, dist, dir);
        pt->lh.curr_line = curr->next_line;
        pt->lh.line_number++;

        #ifdef DEBUG_MOVE
            char _c = PTR_AT_CHR(pt,pt->curr_chr_ptr);
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, 
                "[moving down 1 with col mem (%d)]: xpos: %d, curr: %d, chr_ptr: %ld, dist: %d, line size: %d | [%c]\n",
                curr_col_mem, pos->x, pt->lh.curr_line, pt->curr_chr_ptr, dist, jump_to->line_size, _c
                );
            log_to_file(&sk_logger, pbuf);
        #endif


        // check if at the very bottom and need to trigger a re-render 
        update_view_move_down(pt, lv, pos);
    } 
    return 0;
}