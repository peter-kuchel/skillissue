#include "screen_tests.h"

int test_scrolling_down(){
     return 0;
}

void setup_test_objects(piece_table *pt, FILE* f, termw_info *tinfo, line_view *lv){

     if ( init_piece_table(f, TEST_FILE, pt) < 0 ){
          printf("failed to open test file\n");
	  exit(1); 
     }     

     init_line_handler(&(pt->lh), &(pt->original));
     init_line_view(pt, tinfo, lv, &(pt->lh));
     
     log_piece_table_current(&sk_logger, pt); 
}
int screen_test_scrolling(piece_table *pt, line_view *lv, cursor_pos *pos){
     
     int i;

     // move the cursor 8 places to the right
     for (i = 0; i < 8; i++) sim_pressing_D(pt, pos, lv);

     return 0;
}
int main(){

     piece_table pt; 
     line_view lv;
     cursor_pos pos = { 0, 0 };
     termw_info tinfo = { 8, 76 };
     
     printf("[SCREEN TESTS STARTING]\n");
     FILE* f = fopen(TEST_FILE, "r+");

     setup_logger(&sk_logger, LOG_FILE);
     setup_test_objects(&pt, f, &tinfo, &lv);
     screen_test_scrolling(&pt, &lv, &pos);
		     
     fclose(f); 
     printf("[SCREEN TESTS FINISHED]\n");
     return 0;
}
