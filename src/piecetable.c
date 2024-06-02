#include "piecetable.h"




// int get_curr_pos(piece_table* pt){
//    int curr_pos = pt->
// }
char get_curr_char_by_entry(piece_table* pt, pt_entry* ent, size_t pos){
   
   pt_buffer_t* buf = GET_PT_BUFF(pt, ent->src); 
   char c = buf->text[pos];

   return c;
}

int init_piece_table(FILE* f, char* fn, piece_table* pt){
   
   size_t f_size = file_size(fn);

   char* org_buf = (char*)malloc(f_size * sizeof(char));
   char* add_buf = (char*)malloc(STARTING_ADD_BUF_SIZE * sizeof(char));

   size_t bytes_read = fread(org_buf, sizeof(char), f_size, f);

   if (bytes_read < f_size){
      perror("Something went wrong reading from the file"); 
      return -1;
   }

   if (ferror(f) != 0) return -1;

   memset((char*)pt, 0, sizeof(piece_table));

   pt->original.text = org_buf; 
   pt->original.size = f_size; 

   pt->addition.buf.text = add_buf; 
   pt->addition.buf.size = STARTING_ADD_BUF_SIZE; 

   int* _orger = (int*)malloc(sizeof(int) * DEFAULT_PT_ENT_SIZE);
   pt->table.organizer = _orger; 
   pt->table.ent_cap = DEFAULT_PT_ENT_SIZE;
   pt->table.org_midd = (pt->table.ent_cap >> 1) - 1; 

   pt_entry*  table_ents = (pt_entry*)malloc( sizeof(pt_entry) * DEFAULT_PT_ENT_SIZE);
   pt->table.entries = table_ents; 
   pt->table.ent_cap = DEFAULT_PT_ENT_SIZE;

   // pt_entry** redo_ents = (pt_entry**)malloc( sizeof(pt_entry*) * DEFAULT_PT_ENT_SIZE);
   int* redo_ents = (int*)malloc( sizeof(int) * DEFAULT_PT_ENT_SIZE);
   pt->redo.stack = redo_ents;
   pt->redo.cap = DEFAULT_PT_ENT_SIZE;

   int* undo_ents = (int*)malloc( sizeof(int) * DEFAULT_PT_ENT_SIZE);
   pt->undo.stack = undo_ents; 
   pt->undo.cap = DEFAULT_PT_ENT_SIZE;

   int* reclaim_spots = (int*)malloc( sizeof(int) * DEFAULT_PT_ENT_SIZE);
   pt->reclaim.stack = reclaim_spots;
   pt->reclaim.cap = DEFAULT_PT_ENT_SIZE;

   // add the first element - which is the original buffer 

   int entry_pos = pt->table.ent_num;       // will be 0 because of memset

   pt_entry* org_entry = &(pt->table.entries[entry_pos]);

   org_entry->src = ORGN; 
   org_entry->start = 0; 
   org_entry->len = f_size;

   int middle = pt->table.org_midd; 
   pt->table.organizer[middle] = entry_pos;
   pt->table.org_head = middle; 
   pt->table.org_tail = middle; 
   pt->table.org_num++;  

   pt->curr_org_ptr = middle;
   pt->curr_ins_ent = -1;
   pt->curr_del_ent = -1;
   pt->curr_del_org = -1;
   pt->curr_ins_org = -1;

   // pt->table.ent_num++; 

   return 0; 

}

void empty_piece_table(piece_table* pt){

   safeFree(pt->original.text);
   safeFree(pt->addition.buf.text);

   safeFree(pt->table.organizer);
   safeFree(pt->table.entries); 

   safeFree(pt->redo.stack);
   safeFree(pt->undo.stack);
   safeFree(pt->reclaim.stack);

}

void log_piece_table_current(Logger* logger, piece_table* pt){

   // if (!LOG_TOGGLE) return; 
   
   pt_table_t* tb = &(pt->table);
   
   int pbuf_i; 
   
   add_buffer_t* adds = &(pt->addition);
   log_to_file(logger, "----------------------\nCurrent additions state:\n[");

   pbuf_i = 0;
   memset(pbuf, 0, PBUF_SIZE);
   for (size_t j = 0; j < adds->curr_pos; j++){
      
      pbuf[pbuf_i] = adds->buf.text[j];
      pbuf_i++;

      if ( (j > 0 && j % PBUF_SIZE == 0) || j + 1 == adds->curr_pos){

         log_to_file(logger, pbuf);
         memset(pbuf, 0, PBUF_SIZE);
         pbuf_i = 0; 
      }
      
   }
   log_to_file(logger, "]\n");

   memset(pbuf, 0, PBUF_SIZE); 
   sprintf(pbuf, "\ncurr_chr_ptr: %ld\ncurr_org_ptr: %d\n\ncurr_ins_org: %d\ncurr_ins_ent: %d\n\ncurr_del_org: %d\ncurr_del_ent: %d\n",
               pt->curr_chr_ptr, pt->curr_org_ptr, pt->curr_ins_org, pt->curr_ins_ent,
               pt->curr_del_org, pt->curr_del_ent
   );

   log_to_file(logger, pbuf);

   memset(pbuf, 0, PBUF_SIZE);
   sprintf(pbuf, "\norg_head: %d\norg_tail: %d\n", tb->org_head, tb->org_tail);
   log_to_file(logger, pbuf);

   int i; 
   int _tail = tb->org_tail;
   log_to_file(logger, "Current piece table state:\n");
   for (i = tb->org_head; i <= _tail; i++){
      memset(pbuf, 0, PBUF_SIZE);
      pt_entry* ent = ENT_PTR_AT_POS_IN_ORG(pt, i);

      sprintf(pbuf, "(%s, %ld, %ld) @ [%d]\n", 
                     ent->src == ORGN ? "ORG" : "ADD", ent->start, ent->len, i);
      log_to_file(logger, pbuf);
   }

   
   log_to_file(logger, "Current organizer state:\n[");
   for (i = tb->org_head; i <= tb->org_tail; i++){
      memset(pbuf, 0, PBUF_SIZE);
      if (i == tb->org_tail) 
         sprintf(pbuf, "%d", tb->organizer[i]);
      else 
         sprintf(pbuf, "%d, ", tb->organizer[i]);

      log_to_file(logger, pbuf);
   }
   log_to_file(logger, "]\n");

   log_to_file(logger, "Current undo stack state:\n[");
   for (i = 0; i < pt->undo.ptr; i++){
      memset(pbuf, 0, PBUF_SIZE);
      if (i == pt->undo.ptr - 1) 
         sprintf(pbuf, "%d", pt->undo.stack[i]);
      else 
         sprintf(pbuf, "%d, ", pt->undo.stack[i]);

      log_to_file(logger, pbuf);
   }

   log_to_file(logger, "]\n\n----------\n");



}