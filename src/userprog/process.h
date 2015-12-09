#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

tid_t process_execute (const char *file_name);
struct child_thread_holder* get_child_proc(tid_t child_tid);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

struct args_holder
{
  char* arg_start;
  struct list_elem elem;   
};

#endif /* userprog/process.h */
