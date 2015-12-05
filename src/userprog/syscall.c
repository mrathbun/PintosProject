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
check_valid_pointer (const void* esp)
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

void check_valid_buffer(void* esp, int offset)
{
  char* checkBuffer = *(char**)(esp + offset);
  if(!check_valid_pointer((void*)checkBuffer))
  {
    exit(-1);
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
        halt();
        break;
      case SYS_EXIT:
        exit(*(int*)(esp + 4));   
        break;
      case SYS_EXEC:

        break;
      case SYS_WAIT:

        break;
      case SYS_CREATE:
        check_valid_buffer(esp, 4); 
        result = create(*(char**)(esp + 4), *(int*)(esp + 8));
        f->eax = result;  
        break;
      case SYS_REMOVE:

        break;
      case SYS_OPEN:
        check_valid_buffer(esp, 4);
        result = open(*(char**)(esp + 4));
        f->eax = result;
        break;
      case SYS_FILESIZE:

        break;
      case SYS_READ:

        break;
      case SYS_WRITE:
        check_valid_buffer(esp, 8);
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
  //#ifdef USERPROG
  printf("%s: exit(%d)\n", thread_current()->name, status);
  thread_exit();
  //#endif
  //syscall_exit();
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
  sema_down(&file_sema);
  bool success = filesys_create(file, initial_size);
  sema_up(&file_sema); 
  return success; 
}

bool remove (const char *file)
{
  return false;
}

int open (const char *file)
{
  int fd = -1;
  struct thread* cur = thread_current();
  printf("File is opened\n");
  sema_down(&file_sema);
  struct file* tempFile = filesys_open(file);
  if(tempFile != NULL)
  {
    fd = globalfd++;
    struct file_mapper mapper;
    mapper.fd = fd;
    mapper.file = tempFile;  
 
    list_push_back(&(cur->file_list), &(mapper.elem));
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
  sema_down(&file_sema);
  struct file* file = mapFile(fd);  
  file_close(file);
  sema_up(&file_sema);
}

/*
 * Returns the file that corresponds with the file descriptor fd,
 * or null if there isn't and open file with that descriptor. 
 */
struct file* mapFile(int fd) 
{
  struct list f_list = thread_current()->file_list;
  struct list_elem *e;

  for(e = list_begin (&f_list); e != list_end (&f_list);
      e = list_next (e))
  {
    struct file_mapper* temp = list_entry (e, struct file_mapper, elem);
    if(temp->fd == fd)
    {
      return temp->file;
    }  
  } 

  return NULL;   
}

