#include <stdio.h> 
#include <stdlib.h>
#include <string.h>


#include "skillissue.h"


static termw_info tinfo; 
static Logger logger; 

void save_file_writes(FILE* f, piece_table* pt){
    // put together the piece table 

    printf("%p %p", f, pt);


}

// int insert_at(piece_table* pt, char c){
//     // int res; 

//     text_buffer* add_buff = pt->add_buffer; 

//     /* check that there is space in the add buffer first */
//     if (add_buff->curr_pos + 1 > add_buff->size){
        
//         if (resize_add_buffer(add_buff) < 0){
//             printf("[ERROR]: something went wrong attempting to call realloc... exiting now");
//             return -1; 
//         }
//     }

//     /* check if this is the start of a new insert */
//     if (pt->start_ins_chr == '\0'){
//         pt->start_ins_chr = c; 
//         pt->start_ins_pos = add_buff->curr_pos;
//     }

//     return 0; 
// }

// void esc_make_changes(usermode* umode, piece_table* pt){
//     switch(umode->mode){
//         case USR_MODE_INS:
//             /* add finalized entry to the piece table */
//             // pt->start_ins_chr = '\0';
//             break; 
//         default:
//             break; 
//     }
// }

int edit_file(char* fn){
    char user_in; 
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
        goto handle_error; 
    }
    
    // printw("waiting for input, umode is: %hu\n", umode.mode);
    render_screen(&pt);

    /* set cursor to very beginning of the file */
    cursor_pos pos = { .x = 0, .y = 0 };
    move(pos.y, pos.x);


    do {

        /* render piece table to output */
        
        user_in = getch(); 
        printw("user in is: %c", user_in);

        /* determine the mode first if on is active */
        if (umode.mode > 0){
            
            switch(umode.mode){

                case USR_MODE_INS:
                    if (user_in == USR_MODE_ESC){
                        // esc_make_changes(&umode, &pt);
                        umode.mode &= 0;
                    } else {
                        // res = insert_at(&pt, user_in);
                    }
                    
                    // render_screen(&pt);
                    break;

                default:
                    break; 
            }
        }

        /* if not in a particular mode then see if we'll enter a mode */
        else {
            switch(user_in){
                /* exit the current mode */
                // case USR_MODE_ESC:
                //     /* save changes depending on the mode before */
                //     esc_make_changes(&umode, &pt);
                //     umode.mode &= 0; 
                //     break;

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
                    if (pos.x > 0)
                        pos.x--; 
                    break;
                case 's':

                    break;
                case 'd':

                    // if ()
                    pos.x++; 
                    break; 
                
                default:
                    /* let the user know this was not valid */
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
    

handle_error:
    empty_piece_table(&pt);
    fclose(f); 
    return -1; 
}

void menu_screen(){

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

    
    setup_logger(&logger, LOG_FILE);

    /* run */
    run_sk(argc, argv);

 
    return 0; 
}