#include <stdio.h> 
#include <stdlib.h>
#include <string.h>


#include "skillissue.h"


static termw_info tinfo; 
static Logger logger; 

void save_file_writes(FILE* f, piece_table* pt){
    // put together the piece table 

    printf("%p %p", f, pt);

    // use freopen to clear the contents ?


}

int handle_insertion_mode(piece_table* pt, usermode* umode, char user_in){
    add_buffer_t* add_buf = &(pt->addition);

    /* insert mode was just activated:
        create the entry at current position 
    */
    if ( !(add_buf->write_active) ){
        add_buf->saved_pos = add_buf->curr_pos;
        add_buf->write_active++; 
    }

    /* handle user finishing up insertion mode*/
    if (user_in == USR_MODE_ESC){
        // esc_make_changes(&umode, &pt);
        umode->mode &= 0;
        add_buf->write_active--; 

            
    /* handle when backspace is pressed */
    } else if (user_in == 127) {
        

        /* handle deletions pertaining to the original text */
        if (add_buf->curr_pos == add_buf->saved_pos){
        
        /* re-handle the entry next to the current */
        } else {
            
        }
    
    /* insert char into additions and update the entry*/
    } else {
        add_buf->buf.text[add_buf->curr_pos] = user_in;
        add_buf->curr_pos++;
    }


    return 0; 
}

int handle_side_movement(piece_table* pt, cursor_pos* pos, int dir){
    // determine the next pointed to character 
    // retrieve the current entry and position
    pt_entry* ent; 
    pt_table_t* table; 
    // get current entry and position 
    ent = GET_CURR_ENT_PTR(pt);
    size_t chr_ptr = pt->curr_chr_ptr; 

    printf("%ld\n", chr_ptr);

    // move to the right 
    if (dir > 0){
        size_t upper_bound = ent->start + ent->len;

        // go forward to next entry 
        if (chr_ptr + 1 > upper_bound){

            table = &(pt->table);
            if (table->org_tail == pt->curr_ent_ptr){
                printf("at pos n, can't move\n");
                return 0;
            }

            pt->curr_ent_ptr++; 
            ent = GET_CURR_ENT_PTR(pt);

            pt->curr_chr_ptr = ent->start; 
        } 

        pos->x++;

        // handle a newline
        char curr_chr = get_curr_char_by_entry(pt, ent, pt->curr_chr_ptr);

        if (curr_chr == '\n'){
            pos->x = 0; 
            pos->y++;
        }


    } else {
        size_t lower_bound = ent->start; 

        // go back to prev entry 
        if (chr_ptr - 1 < lower_bound){

            table = &(pt->table);
            if (table->org_head == pt->curr_ent_ptr){
                printf("at pos 0, can't move\n");
                return 0;
            }

            pt->curr_ent_ptr--; 
            ent = GET_CURR_ENT_PTR(pt);

            pt->curr_chr_ptr = ent->start + ent->len; 
        }


        pos->x--; 

        char curr_chr = get_curr_char_by_entry(pt, ent, pt->curr_chr_ptr);

        if (curr_chr == '\n'){

            // need to find position for x at the end of the line 
            pos->x = 0; 
            pos->y--;
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
                    insert_res = handle_insertion_mode(&pt, &umode, user_in);
                    if (insert_res < 0){}
                    break;

                default:
                    break; 
            }
        }

        /* if not in a particular mode then see if we'll enter a mode */
        else {
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

    /* if no save was made then keep the contents as they were when the file was opened */
    if (!umode.made_save){
        
        /* take the original buffer from the pt and write it*/
        // size_t og_size = pt.original_buffer->size; 
        // size_t pos = 0, wr_sz = WRITE_SIZE; 
        // int eow = 0; 

        // do {
                                                
        //     pos = (size_t)ftell(f);
        //     if ( (pos + WRITE_SIZE) < og_size ){
        //         wr_sz = og_size - pos; 
        //         eow++; 
        //     }
            
        //     fwrite(pt.original + pos, sizeof(char), wr_sz, f);
            
        // } while ( !eow );
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

        teardown_logger(&logger);
        endwin();

        if (res < 0) exit(1);

    } else {
        printf("[Error]: need to supply valid file path as arg\n");
        exit(1);
    }
}

int main(int argc, char** argv){

    
    setup_logger(&sk_logger, LOG_FILE);
    log_to_file(&sk_logger, "logger has been successfully set up!\n");

    /* run */
    run_sk(argc, argv);

 
    return 0; 
}