#include "screen_tests.h"

int test_scrolling_down(){
     return 0;
}

void setup_test_objects(piece_table *pt){

}

int main(){
     piece_table pt; 
     line_view lv;
     cursor_pos pos = { 0, 0 };
     termw_info tinfo = { 8, 76 };
     FILE* f = fopen(test_file, "r+");

     setup_logger(&sk_logger, LOG_FILE);
     #ifdef DEBUG_GEN
          memset(pbuf, 0, PBUF_SIZE);
	  sprintf(pbuf, "starting debugging of screen\n");
	  printf("%s", pbuf);
     #endif

     if ( init_piece_table(f, test_file, &pt) < 0 ){
          printf("failed to open test file\n");
	  exit(1); 
     }     
     init_line_handler(&(pt.lh), &(pt.original));
     init_line_view(&pt, &tinfo, &lv, &(pt.lh));
     
     log_piece_table_current(&sk_logger, &pt); 
     return 0;
}
