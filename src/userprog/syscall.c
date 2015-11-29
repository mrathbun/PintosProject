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
    return pagedir_get_page(thread_current()->pagedir, esp) != NULL;
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
    int syscallNum = *(int*)(esp);
    hex_dump(0, esp, 100, true);
    switch(syscallNum) {
      case SYS_HALT:

        break;
      case SYS_EXIT:
        //Add code for passing status to parent
        process_exit();   
        break;
      case SYS_EXEC:

        break;
      case SYS_WAIT:

        break;
      case SYS_CREATE:

        break;
      case SYS_REMOVE:

        break;
      case SYS_OPEN:

        break;
      case SYS_FILESIZE:

        break;
      case SYS_READ:

        break;
      case SYS_WRITE:
        printf("Write called\n");
        esp += 8;
        printf("String written: %s\n", (char*)(esp));
        break;
      case SYS_SEEK:

        break;
      case SYS_TELL:

        break;
      case SYS_CLOSE:

        break;
      default:
        printf("Unexpected syscall\n");
        break;
       
    }
    
  }
  else 
  {
    process_exit();
  } 
  thread_exit ();
}


