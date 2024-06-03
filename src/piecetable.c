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

   pt_entry*  table_ents = (pt_entry*)malloc( sizeof(pt_entry) * DEFAULT_PT_ENT_SIZE);
   pt->entries = table_ents; 
   pt->ent_cap = DEFAULT_PT_ENT_SIZE;

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

   int first_entry = pt->ent_num;       // will be 0 because of memset

   pt_entry* orgn_entry = &(pt->entries[first_entry]);

   orgn_entry->src = ORGN; 
   orgn_entry->start = 0; 
   orgn_entry->len = f_size;

   orgn_entry->left = NULL_ENT;
   orgn_entry->right = NULL_ENT; 

   pt->ent_head = first_entry; 
   pt->ent_tail = first_entry;

   pt->curr_ins_ent = NULL_ENT;
   pt->curr_del_ent = NULL_ENT;

   return 0; 

}

void empty_piece_table(piece_table* pt){

   safeFree(pt->original.text);
   safeFree(pt->addition.buf.text);

   safeFree(pt->entries); 

   safeFree(pt->redo.stack);
   safeFree(pt->undo.stack);
   safeFree(pt->reclaim.stack);

}

void log_piece_table_current(Logger* logger, piece_table* pt){

   // if (!LOG_TOGGLE) return; 
   
   // pt_table_t* tb = &(pt->table);
   
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
   sprintf(pbuf, "\nhead: %d\ntail: %d\ncurr_ent_ptr: %d\ndel_ent: %d\nins_ent: %d\nchr_ptr: %ld\n",
               pt->ent_head, pt->ent_tail, pt->curr_ent_ptr, pt->curr_del_ent, pt->curr_ins_ent,
               pt->curr_chr_ptr
   );

   log_to_file(logger, pbuf);

   // memset(pbuf, 0, PBUF_SIZE);
   // sprintf(pbuf, "\norg_head: %d\norg_tail: %d\n", tb->org_head, tb->org_tail);
   // log_to_file(logger, pbuf);

   int i = 0; 
   int _ent = pt->ent_head; 
   // int _tail = tb->org_tail;
   log_to_file(logger, "\nCurrent piece table state:\n");

   pt_entry* ent; 

   int all_ent_ptrs[pt->ent_num];
   do {
      memset(pbuf, 0, PBUF_SIZE);

      ent = &(pt->entries[_ent]);
      sprintf(pbuf, "[%d] @ ( %d | %s, %ld, %ld | %d ) @ [%d]\n",
               _ent,
               ent->left, 
               ent->src == ORGN ? "ORG": "ADD",
               ent->start,
               ent->len, 
               ent->right, _ent  
      );
      log_to_file(logger, pbuf);

      all_ent_ptrs[i] = _ent; 
      i++; 
      _ent = ent->right;
      
   }while (_ent != NULL_ENT);

   int size = i; 

   log_to_file(logger, "\nCurrent organizer state:\n[");
   for (i = 0; i < size; i++){
      memset(pbuf, 0, PBUF_SIZE);
      if (all_ent_ptrs[i] == pt->ent_tail) 
         sprintf(pbuf, "%d", all_ent_ptrs[i]);
      else 
         sprintf(pbuf, "%d, ", all_ent_ptrs[i]);

      log_to_file(logger, pbuf);
   }

   log_to_file(logger, "]\n");
   // for (i = tb->org_head; i <= _tail; i++){
   //    memset(pbuf, 0, PBUF_SIZE);
   //    pt_entry* ent = ENT_PTR_AT_POS_IN_ORG(pt, i);

   //    sprintf(pbuf, "(%s, %ld, %ld) @ [%d]\n", 
   //                   ent->src == ORGN ? "ORG" : "ADD", ent->start, ent->len, i);
   //    log_to_file(logger, pbuf);
   // }

   
   // log_to_file(logger, "Current organizer state:\n[");
   // for (i = tb->org_head; i <= tb->org_tail; i++){
   //    memset(pbuf, 0, PBUF_SIZE);
   //    if (i == tb->org_tail) 
   //       sprintf(pbuf, "%d", tb->organizer[i]);
   //    else 
   //       sprintf(pbuf, "%d, ", tb->organizer[i]);

   //    log_to_file(logger, pbuf);
   // }
   // log_to_file(logger, "]\n");

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