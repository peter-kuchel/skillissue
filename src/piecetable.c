#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#include "piecetable.h"


int init_piece_table(FILE* f, char* fn, piece_table* pt){
     struct stat fin;

     if ( stat(fn, &fin) < 0){
        printf("[ERROR]: could not get info on file %s\n", fn);
        return -1;
     }

     size_t fsize = (size_t)fin.st_size; 

     char* org_buf = (char*)malloc(sizeof(char) * fsize);
     char* add_buf = (char*)malloc(sizeof(char) * fsize);

     fread(org_buf, sizeof(char), fsize, f);

     text_buffer* org = (text_buffer*)malloc(sizeof(text_buffer));
     text_buffer* add = (text_buffer*)malloc(sizeof(text_buffer));

     org->text = org_buf; 
     org->size = fin.st_size;
     org->curr_pos = -1;         /* -1 since original will be read only (though this doesn't actually matter) */

     add->text = add_buf;
     add->size = fin.st_size; 
     add->curr_pos = 0; 

      pt->original_buffer = org; 
      pt->add_buffer = add; 
      pt->pt_size = DEFAULT_PT_ELEM_SIZE;
      pt->table = (piece_table_element*)malloc(sizeof(piece_table_element) * pt->pt_size); 
      pt->start_ins_chr = '\0';
      pt->start_ins_pos = 0;

      /* first element in the piece table should be the original buffer */
     
   return 0;
}

void empty_piece_table(piece_table* pt){

   free(pt->add_buffer->text);
   free(pt->original_buffer->text);

   free(pt->table);
   free(pt->add_buffer);
   free(pt->original_buffer);

}

int resize_add_buffer(text_buffer* add_buff){
   char* curr_buff = add_buff->text; 

   size_t new_size = add_buff->size << 1; 

   char* new_loc = (char*)realloc(curr_buff, new_size);

   if (new_loc == NULL) 
      return -1; 

   add_buff->text = new_loc; 
   add_buff->size = new_size;

   return 0; 
}