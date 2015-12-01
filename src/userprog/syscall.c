#include "userprog/syscall.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <console.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "devices/shutdown.h"
#include "filesys/filesys.h"

static void syscall_handler (struct intr_frame *);

static struct semaphore file_sema;
static int globalfd;

void
syscall_init (void) 
{
  sema_init(&file_sema, 1);
  globalfd = 2;
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
  //printf ("system call!\n");
  void *esp = f->esp;
  if(check_valid_pointer(esp))
  {
    int syscallNum = *(int*)(esp);
    int result;
    //hex_dump(0, esp, 100, true);
    switch(syscallNum) {
      case SYS_HALT:

        break;
      case SYS_EXIT:
        exit(*(int*)(esp + 4));   
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
        result = open(*(char**)(esp + 4));
        printf("File num: %d\n", result);
        f->eax = result;
        break;
      case SYS_FILESIZE:

        break;
      case SYS_READ:

        break;
      case SYS_WRITE:
        result = write(*(int*)(esp + 4), esp + 8, *(int*)(esp + 12));  
        f->eax = result;
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
    thread_exit();
  } 
}

void halt (void)
{
  shutdown_power_off();
}

void exit (int status)
{
  //Handle passing exit code to parent
  //printf("args-none: exit(%d)\n", status);
  thread_exit();
}

int exec (const char *file)
{
  return 0;
}

int wait (int childProc)
{
  return 0;
}

bool create (const char *file, unsigned initial_size)
{
  return false; 
}

bool remove (const char *file)
{
  return false;
}

int open (const char *file)
{
  int fd = -1;
  sema_down(&file_sema);
  struct file* tempFile = filesys_open(file);
  if(tempFile != NULL)
  {
    fd = globalfd++;
    struct file_mapper mapper;
    mapper.fd = fd;
    mapper.file = file;  
 
    list_push_back(&thread_current()->file_list, &mapper.elem);
  }
  
  sema_up(&file_sema);     
  return fd; 
}

int filesize (int fd){
  return 0;
}

int read (int fd, void *buffer, unsigned length)
{
  return 0;
}

int write (int fd, const void *buffer, unsigned length)
{
  if(fd == 1) 
  {
    putbuf(*(char**)buffer, length);
    return length;
  }
  else 
  {
    return 0;
  }
}

void seek (int fd, unsigned position)
{
  return;
}

unsigned tell (int fd)
{
  return 0;
}

void close (int fd)
{
  return;
}

