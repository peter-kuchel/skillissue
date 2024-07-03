#include "cursor.h"

// TODO: all this code should be refactored because its ugly af 


// for when a or d is pressed 
int handle_side_movement(piece_table* pt, cursor_pos* pos, int dir, line_view* lv){
    
    // reset the column memory when moving across the line
    pt->lh.col_mem = -1; 

    pt_entry* ent = &(pt->entries[pt->curr_ent_ptr]);
    size_t chr_ptr = pt->curr_chr_ptr; 

    /* move to the right */  
    if (dir > 0){
        size_t upper_bound = ent->start + ent->len;

        // go forward to next entry 
        if (chr_ptr == upper_bound || chr_ptr + 1 == upper_bound){
            
            #ifdef DEBUG_MOVE
                memset(pbuf, 0, PBUF_SIZE);
                sprintf(pbuf, "[At (or past) upper bound]\n");
                log_to_file(&sk_logger, pbuf);
            #endif 

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
        

    /* move to the left */ 
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
                return 0;
            }
            
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

    }

    return 0; 
}

int handle_jump_down(piece_table* pt, cursor_pos* pos, int prev_chr_ptr, int prev_ent, line_view* lv){

    pt_src_t _prev_src = (pt->entries[prev_ent]).src;
    pt_buffer_t *src_buffer = GET_PT_BUFF(pt, _prev_src);
    char prev_char = src_buffer->text[prev_chr_ptr]; 

    #ifdef DEBUG_MOVE
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, 
            "[JUMP TEST DOWN]: %x\n", prev_char);
        log_to_file(&sk_logger, pbuf);
    #endif 

    if ( prev_char == '\n'){
                        
        line *curr = LH_CURR_LINE(pt);
        pt->lh.curr_line = curr->next_line; 
        pos->y++;
        pos->x = 0;
        
        #ifdef DEBUG_MOVE
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, 
                "[JUMP DOWN]: curr line is: %d\n", pt->lh.curr_line);
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

    if ( _c == '\n'){
        
        line *curr = LH_CURR_LINE(pt);
        pt->lh.curr_line = curr->prev_line; 
        pos->y--; 
        pos->x = CURR_LINE_SIZE(pt);

        #ifdef DEBUG_MOVE
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, 
                "[JUMP UP]: curr line is: %d\n", pt->lh.curr_line);
            log_to_file(&sk_logger, pbuf);
        #endif
        
    }
    return 0; 
}

int move_chr_ptr(piece_table* pt, int dist, int dir){

    int d = 0; 
    int curr_ent = pt->curr_ent_ptr; 
    
    pt_entry *curr = &(pt->entries[curr_ent]);
    size_t end = dir > 0 ? curr->start + (curr->len - 1) : curr->start; 

    if ( curr_ent == pt->ent_tail && 
         pt->curr_chr_ptr == curr->start + (curr->len - 1) ){
        pt->curr_chr_ptr--;
    }

    while (d <= dist){

        #ifdef DEBUG_MOVE
            char _c = PTR_AT_CHR(pt,pt->curr_chr_ptr);
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, 
                "[dir: %d] %d <= %d (dist) | at: {%ld} -> end {%ld}| curr_ent: %d [%c]\n",
                dir, d, dist, pt->curr_chr_ptr, end, curr_ent, _c  
            );
            log_to_file(&sk_logger, pbuf);
        #endif 

        if (pt->curr_chr_ptr == end){

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
                end = dir > 0 ? curr->start + (curr->len - 1) : curr->start;

                #ifdef DEBUG_MOVE
                    memset(pbuf, 0, PBUF_SIZE);
                    sprintf(pbuf, 
                        "[REACH END]: new-- end: %ld, curr_ent: %d, curr_chr_ptr: %ld\n",
                            end, curr_ent, pt->curr_chr_ptr
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
  
    int _curr_line = pt->lh.curr_line; 
    curr = &(pt->lh.lines[_curr_line]);
    curr_size = curr->line_size;

    // going up the file
    if (dir < 0){

        if (pt->lh.top_line == _curr_line) return 0; 

        pos->y--; 

        jump_to = &(pt->lh.lines[curr->prev_line]); 
        jump_size = jump_to->line_size;

        //  
        if (jump_size >= curr_col_mem){
            dist = pos->x + (jump_size - curr_col_mem); 
            pos->x = curr_col_mem;
        } else{
            dist = pos->x;
            pos->x = jump_size;
        } 
        
        move_chr_ptr(pt, dist, dir);
        pt->lh.curr_line = curr->prev_line;

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

    // check for going down the file
    } else {

        if (pt->lh.bottom_line == _curr_line) return 0;

        pos->y++; 

        jump_to = &(pt->lh.lines[curr->next_line]);
        jump_size = jump_to->line_size;

        if (jump_size >= curr_col_mem){
            dist = curr_col_mem + (curr_size - pos->x);
            pos->x = curr_col_mem;
        } 
        
        else{
            dist = jump_size + (curr_size - pos->x); 
            pos->x = jump_size;
        }
        

        move_chr_ptr(pt, dist, dir);
        pt->lh.curr_line = curr->next_line;

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