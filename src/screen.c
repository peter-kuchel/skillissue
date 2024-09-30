#include "screen.h"


void init_line_view(piece_table* pt, termw_info* tinfo, line_view* lv, line_handler* lh){

    memset((char*)lv, 0, sizeof(line_view)); 

    lv->top_win = lh->top_line;

    // there are more rows than can be displayed
    if (tinfo->rows < lh->size){
        
        line* _l; 
        int curr_l = lh->top_line;

        for(int v = 0; v < tinfo->rows - 1; v++){
            _l = &(lh->lines[curr_l]);
            curr_l = _l->next_line;
        }

        lv->bot_win = curr_l;

    } else {
        lv->bot_win = lh->bottom_line; 
    }

    // lv->left_win is already 0 
    lv->right_win = tinfo->cols; 
    lv->tinfo_ptr = tinfo;

    lv->top_view_ent = pt->ent_head; 
    lv->top_view_chr = pt->curr_chr_ptr; 

    #ifdef DEBUG_SCREEN
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, 
            "[line init]: top_win: %d, bot_win: %d, right_win: %d, left_win: %d\n\t\t\ttop view ent: (%d), top view chr_ptr: (%ld)\n", 
            lv->top_win, lv->bot_win, lv->right_win, lv->left_win, lv->top_view_ent, lv->top_view_chr
            );
        log_to_file(&sk_logger, pbuf);
    #endif
     
}

static void update_top_ent_up(piece_table* pt, line_handler* lh, line_view* lv){

    line* new_top = LINE_AT_POS(lh, lv->top_win); 
    pt_entry* ent = ENT_AT_POS(pt, lv->top_view_ent); 

    // avoid possible underflow when chr ptr is at the beginning of either text buffer 
    ssize_t chr_ptr = (ssize_t)lv->top_view_chr; 
    ssize_t ent_start = (ssize_t)ent->start;

    // line_size + 1 to account for the new line and skip over it
    for(int d = 0; d < new_top->line_size + 1; d++){

        chr_ptr--; 
        if (chr_ptr < ent_start){

            lv->top_view_ent = ent->left; 
            ent = ENT_AT_POS(pt, lv->top_view_ent);

            chr_ptr = (ssize_t)(ent->start + ent->len - 1);
            ent_start = (ssize_t)ent->start; 
        }
    }

    
    lv->top_view_chr = (size_t)chr_ptr;
}

static void update_top_ent_down(piece_table* pt, line_handler* lh, line_view* lv, int prev_top_win){
    
    line* prev_top = &(lh->lines[prev_top_win]);
    pt_entry* ent = ENT_AT_POS(pt, lv->top_view_ent);

    // line_size + 1 to account for the new line and skip over it
    for (int d = 0; d < prev_top->line_size + 1; d++){

        lv->top_view_chr++; 

        if (lv->top_view_chr == (ent->start + ent->len)){

            lv->top_view_ent = ent->right; 
            ent = ENT_AT_POS(pt, lv->top_view_ent);
            lv->top_view_chr = ent->start; 
        }
    }
}

void update_view_move_down(piece_table* pt, line_view* lv, cursor_pos* pos){
    
    line_handler* lh = &(pt->lh);

    int max_row = lv->tinfo_ptr->rows;

    // check if the screen lines will be shifted
    if (pos->y == max_row){

        if (lv->bot_win != lh->bottom_line){ 

            int prev_top_win = lv->top_win;
            lv->top_win = (lh->lines[lv->top_win]).next_line;  
            lv->bot_win = (lh->lines[lv->bot_win]).next_line;

            // update line views top_view_ent 
            update_top_ent_down(pt, lh, lv, prev_top_win);
            lv->needs_render++; 
        }

        pos->y--; 
        
    }
    
}

void update_view_move_up(piece_table* pt, line_view* lv, cursor_pos* pos){

    line_handler* lh = &(pt->lh);

    // check if the screen lines will be shifted
    if (pos->y < 0){

        if (lv->top_win != lh->top_line){

            lv->top_win = (lh->lines[lv->top_win]).prev_line; 
            lv->bot_win = (lh->lines[lv->bot_win]).prev_line; 

            update_top_ent_up(pt, lh, lv);    

            lv->needs_render++;
        }

        pos->y++;
        
    }
}

void update_view_move_left(line_view* lv, cursor_pos* pos){

    #ifdef DEBUG_SCREEN
        memset(pbuf, 0, PBUF_SIZE); 
        sprintf(pbuf, "[shift window left] -- x: %d, left win: %d\n", pos->x, lv->left_win);
        log_to_file(&sk_logger, pbuf);
    #endif

    if (pos->x < 0){

        if (lv->left_win > 0){

            lv->left_win--;
            lv->right_win--; 
            lv->needs_render++;   
        }
        
        pos->x++;
    }
}

void update_view_move_right(line_view* lv, cursor_pos* pos){

    if (pos->x == lv->tinfo_ptr->cols){

        lv->left_win++;
        lv->right_win++;

        lv->needs_render++;

        pos->x--;
    }
}

void update_view_ins_nl(piece_table* pt, line_view* lv){
    line_handler* lh = &(pt->lh);

    // update bottom window depending on lines file relative to term window size
    if (lh->size < lv->tinfo_ptr->rows){

        lv->bot_win = lh->bottom_line;

    } else {

        lv->bot_win = (lh->lines[lv->bot_win]).prev_line;
    }

}


void update_view_del_nl(piece_table* pt, line_view* lv){

    // only the lv->bot_win gets changed?

    lv->bot_win = (pt->lh.lines[lv->bot_win]).prev_line; 
}


static void init_track(piece_table* pt, line_view* lv, pt_track* track){

    track->curr_start_ptr = lv->top_view_chr; 
    track->curr_ent = lv->top_view_ent;
    track->ent_ptr = ENT_AT_POS(pt, track->curr_ent); 
    track->buff = GET_PT_BUFF(pt, track->ent_ptr->src); 

    // don't need to minus 1 here because we are checking when it goes past entry window
    track->ent_chr_end = track->ent_ptr->start + track->ent_ptr->len;
}

static int move_into_next_ent(piece_table* pt, pt_track* track){

    if (track->curr_start_ptr == track->ent_chr_end){ 

        #ifdef DEBUG_SCREEN
            char last_c = CHR_IN_TRACK( track );
            memset(pbuf, 0, PBUF_SIZE); 
            sprintf(pbuf, 
                    "Last char: %x, chr_ptr: %ld\n", 
                    last_c, track->curr_start_ptr);
            log_to_file(&sk_logger, pbuf);
        #endif

        /*
        only to handle the case 
        for the very last line in the tail entry so that it doesn't enter into entry '-1'
        and random characters are not displayed on the terminal window  
        */
        if (track->curr_ent == pt->ent_tail)
            return 0; 
        
        int next_ent = track->ent_ptr->right;

        track->ent_ptr = ENT_AT_POS(pt, next_ent); 
        track->curr_ent = next_ent; 

        track->buff = GET_PT_BUFF(pt, track->ent_ptr->src);
        track->ent_chr_end = track->ent_ptr->start + track->ent_ptr->len;
        track->curr_start_ptr = track->ent_ptr->start; 

        #ifdef DEBUG_SCREEN
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, 
                "NEXT ENT: %d [start: %ld | end: %ld]\n", 
                next_ent, track->curr_start_ptr, track->ent_chr_end );
            log_to_file(&sk_logger, pbuf);
        #endif
    }

    return 1; 

}

// move the render track's char ptr for parts of the lines cutoff by the rhs
static void move_right_offset(piece_table* pt, pt_track* track, line_print* lp){

    if (lp->right_cutoff){

        #ifdef DEBUG_SCREEN
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, "[cutoff found] - moving chr ptr | before: %ld\n", track->curr_start_ptr);
            log_to_file(&sk_logger, pbuf);
        #endif

        char curr_chr = CHR_IN_TRACK(track);

        pt_entry *last_ent = ENT_AT_POS(pt, pt->ent_tail);
        size_t last_char_pos = last_ent->start + last_ent->len; 

        while (curr_chr != '\n' && track->curr_start_ptr != last_char_pos){

            move_into_next_ent(pt, track);

            track->curr_start_ptr++; 
            curr_chr = CHR_IN_TRACK(track);
        }

        #ifdef DEBUG_SCREEN
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, "[cutoff accounted for] chr ptr: %ld\n\n", track->curr_start_ptr);
            log_to_file(&sk_logger, pbuf);
        #endif

        lp->right_cutoff--; 

        // skip over the '\n' char, since next line doesn't need to have a new line to be displayed to the next line
        track->curr_start_ptr++;
    }
}

static void calc_line_size(line_print* lp, line_view* lv, line* _l){
    lp->line_pos = 0;

    int rhs_bound = lv->tinfo_ptr->cols; 

    // needs to also take into account current right and left window views

    int remaining_line = _l->line_size - lv->left_win;                  

    if (remaining_line < 0)
        remaining_line = 0; 

    // remaining_line++; // +1 to account for eol insertion space and or '\n' char 

    if ( (remaining_line + 1) > rhs_bound){
        lp->right_cutoff++;
        lp->line_size = rhs_bound;

    } else {
        if (remaining_line > 0)
            lp->line_size = remaining_line + 1;
        else 
            lp->line_size = 0;
    }

    // uncomment to see line sizes found
    #ifdef DEBUG_SCREEN
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "[Render line size: %d] -- {rhs bound: %d} -- {original size: %d}\n", lp->line_size, rhs_bound, _l->line_size);
        log_to_file(&sk_logger, pbuf);
    #endif 
}



void display_screen_info(piece_table* pt, line_view* lv, cursor_pos* pos, usermode* umode, char user_in){

    int max_info_size = lv->tinfo_ptr->cols;
    char info_str[ max_info_size ];

    char* mode_str; 
    switch (umode->mode){
        case MODE_INSERT:
            mode_str = "INSERT";
            break;
        case MODE_SAVE:
            mode_str = "SAVE";
            break;
        default:
            break;

    }

    if (umode->mode == MODE_NONE){
        switch (user_in){
            case 'w':
            case 'a':
            case 's':
            case 'd':
                mode_str = "MOVE";
                break; 
            default:
                mode_str = "";
                break;
        }
    }

    int line_num = pt->lh.line_number + 1;

    int size = sprintf(info_str, "[ %d ] [ %d ]\t-{ %s }-", line_num, pos->x, mode_str);
    memset(info_str + size, ' ', max_info_size - size -1 );
    info_str[ max_info_size ] = '\0';

    mvprintw(lv->tinfo_ptr->rows, 0, "%s", info_str);
}

void render_screen(piece_table* pt, line_view* lv){

    // better than using clear()
    // since this stops flickering when rendering back to back 
    erase();

    line_handler* lh = &(pt->lh);
    line *_l; 
    line_print lp = { 0, 0, 0 }; 
 
    int view_curr = lv->top_win;
    
    pt_track track; 
    init_track(pt, lv, &track); 

    int term_rows = lv->tinfo_ptr->rows; 
    int lines_to_render = lh->size < term_rows ? lh->size : term_rows; 

    int i, l_size, c = view_curr;
    int buf_size = 1;               // 1 to account for null terminator

    // calulate the size of the buffer to render beforehand 
    for (i = 0; i < lines_to_render; i++){
        _l = &(lh->lines[c]);

        // lines with just \n have a size of 0, so we need to make space to print \n to the screen
        calc_line_size(&lp, lv, _l);
        l_size = lp.line_size + 1; 
       
        buf_size += l_size;

        c = _l->next_line; 
        
    }

    char print_buf[buf_size]; 
    memset(print_buf, 0, buf_size);
    
    lp.line_size = 0; 
    lp.right_cutoff = 0; 
    
    #ifdef DEBUG_SCREEN
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, 
            "\n----------\n{ Line start: %d | line end: %d | start ent: %d @ (%ld, %ld) }\nLines to render: %d\n", 
            view_curr, lv->bot_win, track.curr_ent, track.curr_start_ptr, track.ent_chr_end, lines_to_render);
        log_to_file(&sk_logger, pbuf);
    
        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, 
            "(top win: %d | bot win: %d | left: %d | right: %d)\n", 
            lv->top_win, lv->bot_win, lv->left_win, lv->right_win);
        log_to_file(&sk_logger, pbuf);

        memset(pbuf, 0, PBUF_SIZE);
        sprintf(pbuf, "print_buf size: %d\n----------\n", buf_size);
        log_to_file(&sk_logger, pbuf);
    #endif 

    int pb_i = 0;                                   // index for print buffer
    for (i = 0; i < lines_to_render; i++){
        _l = &(lh->lines[view_curr]);
        int l_size = _l->line_size;

        calc_line_size(&lp, lv, _l);


        // account for left offset by moving the char ptr if line is not originally just '\n'
        if (l_size > 0){

            int left_off = (l_size > lv->left_win) ? lv->left_win : l_size;

            #ifdef DEBUG_SCREEN
                memset(pbuf, 0, PBUF_SIZE);
                sprintf(pbuf, "[Left offset]: %d\n", left_off);
                log_to_file(&sk_logger, pbuf);
            #endif 

            for (int l_off = 0; l_off < left_off; l_off++){
                track.curr_start_ptr++; 
                move_into_next_ent(pt, &track);
            }
        }

        int ent_ok; 

        #ifdef DEBUG_SCREEN
            memset(pbuf, 0, PBUF_SIZE);
            sprintf(pbuf, "[Line Print]: ( size: %d |  pos: %d | cutoff: %d) \n", lp.line_size, lp.line_pos, lp.right_cutoff);
            log_to_file(&sk_logger, pbuf);
        #endif 

        // account for lines which have just '\n'
        if (lp.line_size == 0)
            lp.line_size++;

        while (lp.line_pos < lp.line_size){

            ent_ok = move_into_next_ent(pt, &track);                        // move into next ent if needed

            if (ent_ok){
                print_buf[pb_i++] = CHR_IN_TRACK( (&track) ); 

                #ifdef DEBUG_SCREEN
                    memset(pbuf, 0, PBUF_SIZE);
                    sprintf(pbuf, "%x ", CHR_IN_TRACK( (&track) ));
                    log_to_file(&sk_logger, pbuf);
                #endif 
                
                track.curr_start_ptr++;
            }
            
            lp.line_pos++; 
        }

        // moving the chr ptr to the next top line by skipping over everything cutoff on the right
        move_right_offset(pt, &track, &lp);

        view_curr = _l->next_line; 
    }

    printw("%s", print_buf);
    
}