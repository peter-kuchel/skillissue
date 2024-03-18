#include <stdio.h> 
#include <stdlib.h>

#include "skillissue.h"
#include "utils.h"

static termw_info tinfo; 

void save_file_writes(FILE* f, piece_table* pt){

}

int insert_at(piece_table* pt, char c){
    int res; 

    text_buffer* add_buff = pt->add_buffer; 

    /* check that there is space in the add buffer first */
    if (add_buff->curr_pos + 1 > add_buff->size){
        
        if (resize_add_buffer(add_buff) < 0){
            printf("[ERROR]: something went wrong attempting to call realloc... exiting now");
            return -1; 
        }
    }

    /* check if this is the start of a new insert */
    if (pt->start_ins_chr == '\0'){
        pt->start_ins_chr = c; 
        pt->start_ins_pos = add_buff->curr_pos;
    }

    return 0; 
}

void esc_make_changes(usermode* umode, piece_table* pt){
    switch(umode->mode){
        case USR_MODE_INS:
            /* add finalized entry to the piece table */
            pt->start_ins_chr = '\0';
            break; 
        default:
            break; 
    }
}

void edit_file(char* fn){
    char user_in; 
    usermode umode; 
    umode.mode &= 0; 

    int in_edit = 1;

    /* check that the file exists */

    /* create appropriate items for reading file and info */
    piece_table pt; 
    FILE* f = fopen(fn, "w+");;
    int res; 
    
    /* open the file and init the piece table */
    if ((res = init_piece_table(f, fn, &pt)) < 0 ) return;

    printw("waiting for input, umode is: %hu\n", umode.mode);
    do {

        /* render piece table to output */
        
        user_in = getch(); 
        printw("user in is: %c", user_in);

        /* determine the mode first if on is active */
        if (umode.mode){
             
            switch(umode.mode){

                case USR_MODE_INS:
                    res = insert_at(&pt, user_in);
                    render_screen(&pt);
                    break;

                default:
                    break; 
            }

        }

        /* if not in a particular mode then see if we'll enter a mode */
        else {
            switch(user_in){

                /* exit the current mode */
                case USR_MODE_ESC:
                    /* save changes depending on the mode before */
                    esc_make_changes(&umode, &pt);
                    umode.mode &= 0; 
                    break;
                
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
                
                /* move around file*/
                case 'w':
                    break; 
                case 'a':
                    break;
                case 's':
                    break;
                case 'd':
                    break; 
                
                default:
                    /* let the user know this was not valid */
                    break;
            }
        }

        /* re-render the screen */

        if (res < 0) in_edit--;
        
    } while (in_edit);

    /* if no save was made then write original contents to the file */
    if (!umode.made_save){
        
        /* take the original buffer from the pt and write it*/
        size_t og_size = pt.original_buffer->size; 
        size_t pos = 0, wr_sz = WRITE_SIZE; 
        int eow = 0; 

        do {
                                                
            pos = (size_t)ftell(f);
            if ( (pos + WRITE_SIZE) < og_size ){
                wr_sz = og_size - pos; 
                eow++; 
            }
            
            fwrite(pt.original_buffer + pos, sizeof(char), wr_sz, f);
             
        } while ( !eow );
    } 

    empty_piece_table(&pt);

    // maybe a goto here incase something goes wrong 
    fclose(f);
}

void menu_screen(){

}

void run_sk(int argc, char** argv){

    /* init terminal screen */
    initscr(); 

    // get size of the terminal 
    getmaxyx(stdscr, tinfo.rows, tinfo.cols); 

    /* started with a file so load this file right away */
    if (argc > 1){
        edit_file(argv[1]);
    } 
    
    /* otherwise goto menu screen */
    else {
        printw("%d %d", tinfo.rows, tinfo.cols);
        menu_screen();
    }

    /* end terminal screen and return back */
    endwin();
    
}

int main(int argc, char** argv){

    

    /* run */
    run_sk(argc, argv);
 
    
    return 0; 
}