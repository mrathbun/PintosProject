#include "userprog/syscall.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

bool 
check_valid_pointer (void* esp)
{
  if(is_user_vaddr(esp)) 
  {
    return pagedir_get_page(thread_current()->pagedir, esp) == NULL;
  }
  else
  {
    return false;
  }
   
}

static void
syscall_handler (struct intr_frame *f) 
{
  printf ("system call!\n");
  void *esp = f->esp;
  if(check_valid_pointer(esp))
  {
    //Handle syscall
  
  }
  else 
  {
    process_exit();
  } 
  thread_exit ();
}


