#include <stdio.h> 
#include <stdlib.h>
#include <string.h>


#include "skillissue.h"


static termw_info tinfo; 


void save_file_writes(FILE* f, piece_table* pt){
    // put together the piece table 

    printf("%p %p", f, pt);

    // use freopen to clear the contents 


}

int handle_insertion_mode(piece_table* pt, usermode* umode, cursor_pos* curs_pos, char user_in){

    pt_entry* curr_ent;

    /* handle user finishing up insertion mode*/
    if (user_in == USR_MODE_ESC){
        
        if (pt->insert_ready) pt->insert_ready--;
        umode->mode &= 0;
         
    /* handle when backspace is pressed */
    } else if (user_in == 127) {
        
    
    /* insert char into additions and update the entry*/
    } else {

        // create the entries needed to handle the insert 
        if ( !pt->insert_ready ){
            
            int ent_ptr = pt->curr_org_ptr; 
            size_t chr_ptr = pt->curr_chr_ptr; 
            curr_ent = CURR_ORG_ENT_PTR(pt);

            // if cursor is at the very end or beggining of the file
            int very_end = ent_ptr == pt->table.org_tail && (curr_ent->start + curr_ent->len) == chr_ptr;
            int very_beginning = ent_ptr == pt->table.org_head && curr_ent->start == chr_ptr;

            int at_ent_end = (pt->curr_chr_ptr + 1) == (curr_ent->start + curr_ent->len); 
            if ( very_end || very_beginning ){

                

                // check if entries need to be reallocd 
                if (pt->table.ent_num + 1 == pt->table.ent_cap){

                }

                pt->table.ent_num++;
                int new_ent_pos = pt->table.ent_num;

                pt_entry* new_single_entry = &(pt->table.entries[new_ent_pos]);
                new_single_entry->src = ADD; 
                new_single_entry->start = pt->addition.curr_pos; 
                new_single_entry->len = 0; 

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
            
            /* handle case where curr_chr_ptr is at the end of an existing entry */
            } else if (at_ent_end){
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


    }


    return 0; 
}



int edit_file(char* fn){
     
    usermode umode; 
    umode.mode &= 0; 

    int in_edit = 1;

    /* check that the file exists */
    if (file_exists(fn) < 0){
        printf("[ERROR]: file could not be located %s\n", fn);
        return -1;
    } 

    /* create appropriate items for reading file and info */
    piece_table pt; 
    FILE* f = fopen(fn, "r+");

    /* check that the file was opened correctly*/
    if (f == NULL){
        printf("[ERROR]: something went wrong with opening the file %s\n", fn);
        return -1;
    }
    
    int edit_status;

    /* open the file and init the piece table */
    if ((edit_status = init_piece_table(f, fn, &pt)) < 0 ){
        printf("Something went wrong trying to create piecetable for the file content\n");
        // goto handle_error; 
        exit(1);
    }
    
    render_screen(&pt);

    /* set cursor to very beginning of the file */
    cursor_pos pos = { .x = 0, .y = 0 };
    move(pos.y, pos.x);

    char user_in;
    // int 
    // pt_entry ent; 
    do {

        /* render piece table to output */
        
        user_in = getch(); 
        printw("user in is: %c", user_in);
        int insert_res; 

        /* determine the mode first if one is active */
        if (umode.mode > 0){
            
            switch(umode.mode){

                case USR_MODE_INS:
                    insert_res = handle_insertion_mode(&pt, &umode, &pos, user_in);
                    if (insert_res < 0){}
                    break;

                default:
                    break; 
            }

        /* if not in a particular mode then see if we'll enter a mode */
        } else {
            switch(user_in){

                /* save progress and write to file */
                case 'z':
                    save_file_writes(f, &pt);
                    umode.made_save = 1;
                    break; 

                /* quit the editor (if not saved then all progress is lost) */
                case 'q':
                    in_edit--; 
                    break;

                /* insert */
                case 'i':
                    umode.mode |= USR_MODE_INS;
                    break;  
                
                /* redo */
                case 'j':
                    break; 
                
                /* undo */
                case 'k':
                    break; 
                
                /* move around file*/
                case 'w':
                    break; 
                    
                case 'a':
                    handle_side_movement(&pt, &pos, -1);
                    break;
                case 's':

                    break;
                case 'd':
                    handle_side_movement(&pt, &pos, 1);
                    break; 
                
                default:
                    /* let the user know this was not valid - or maybe do nothing? */
                    break;
            }
        }

        /* re-render the screen */
        render_screen(&pt);
        move(pos.y , pos.x);

        if (edit_status < 0) in_edit--;
        
    } while (in_edit);

    log_piece_table_current(&sk_logger, &pt);

    /* if no save was made then keep the contents as they were when the file was opened */
    if (!umode.made_save){
        
        fclose(f);
    } 

    empty_piece_table(&pt);

    return 0; 
}

void run_sk(int argc, char** argv){

    /* started with a file so load this file right away */
    if (argc > 1){
        initscr(); 

        // get size of the terminal 
        getmaxyx(stdscr, tinfo.rows, tinfo.cols);
        int res = edit_file(argv[1]);

        teardown_logger(&sk_logger);
        endwin();

        if (res < 0) exit(1);

    } else {
        printf("[Error]: need to supply valid file path as arg\n");
        exit(1);
    }
}

int main(int argc, char** argv){

    
    setup_logger(&sk_logger, LOG_FILE);
    log_to_file(&sk_logger, "logger has been successfully set up!\n===================================\n\n");

    /* run */
    run_sk(argc, argv);

 
    return 0; 
}
/*
aaaaaaaaa
*/