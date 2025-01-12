#include "insertmode_tests.h"


int i_simple_insert(piece_table *pt, line_view *lv, cursor_pos *pos){
    printf("i_simple_insert -- ");
    return 1;
}

int all_insertionmode_tests(piece_table *pt, line_view *lv, cursor_pos *pos){


     printf("\e[0;32m--- [INSERTION MODE TESTS STARTING] ---\e[0m\n");
     
     int res;

     res = i_simple_insert(pt, lv, pos);
     if (res == 0)
	  goto teardown;
     else  
	  printf("\e[0;32m[ TEST PASSED ]\e[0m\n"); 


     printf("-\e[0;32m-- [INSERTION MODE TESTS FINISHED] ---\e[0m\n\n\n");
     return 0;
teardown:
     empty_piece_table(pt);
     exit(1);
}
