#include "userprog/syscall.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <console.h>
#include <string.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/malloc.h"
#include "devices/shutdown.h"
#include "devices/input.h"
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

/*
 *Should this be more robust?
 */
void check_valid_buffer(void* esp, int offset)
{
  if(!check_valid_pointer((void*)*(char**)(esp + offset)))
  {
    exit(-1);
  }
}

void check_valid_args(void* esp, int numArgs) 
{
  int i;
  for(i = 1; i <= numArgs; i++)
  {
    if(!check_valid_pointer(esp + 4 * i))
    {
      exit(-1);  
    } 
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
    //printf("Syscall: %d\n", syscallNum);
    int result;
    //hex_dump(0, esp, 100, true);
    switch(syscallNum) {
      case SYS_HALT:
        halt();
        break;
      case SYS_EXIT:
        check_valid_args(esp, 1);
        exit(*(int*)(esp + 4));   
        break;
      case SYS_EXEC:
        check_valid_args(esp, 1);
        check_valid_buffer(esp, 4);
        result = exec(*(char**)(esp + 4));  
        f->eax = result;  
        break;
      case SYS_WAIT:

        break;
      case SYS_CREATE:
        check_valid_args(esp, 2);
        check_valid_buffer(esp, 4); 
        result = create(*(char**)(esp + 4), *(int*)(esp + 8));
        f->eax = result;  
        break;
      case SYS_REMOVE:
        check_valid_args(esp, 1);
        check_valid_buffer(esp, 4);
        result = remove(*(char**)(esp + 4));
        f->eax = result;
        break;
      case SYS_OPEN:
        check_valid_args(esp, 1);
        check_valid_buffer(esp, 4);
        result = open(*(char**)(esp + 4));
        f->eax = result;
        break;
      case SYS_FILESIZE:
        check_valid_args(esp, 1);
        result = filesize(*(int*)(esp + 4));
        f->eax = result; 
        break;
      case SYS_READ:
        check_valid_args(esp, 3);
        check_valid_buffer(esp, 8); 
        result = read(*(int*)(esp + 4), esp + 8, *(int*)(esp + 12));
        f->eax = result; 
        break;
      case SYS_WRITE:
        check_valid_args(esp, 3);
        check_valid_buffer(esp, 8);
        result = write(*(int*)(esp + 4), esp + 8, *(int*)(esp + 12));  
        f->eax = result;
        break;
      case SYS_SEEK:
        check_valid_args(esp, 2);
        seek(*(int*)(esp + 4), *(int*)(esp + 8));
        break;
      case SYS_TELL:
        check_valid_args(esp, 1);
        result = tell(*(int*)(esp + 4));
        f->eax = result;
        break;
      case SYS_CLOSE:
        check_valid_args(esp, 1);
        close(*(int*)(esp + 4));       
        break;
      default:
        printf("Unexpected syscall\n");
        break;
    }
  }
  else 
  {
    exit(-1);
  } 
}

void halt (void)
{
  shutdown_power_off();
}

void exit (int status)
{
  //Handle passing exit code to parent
  printf("%s: exit(%d)\n", thread_current()->name, status);
  thread_exit();
}

int exec (const char *file)
{
  sema_down(&file_sema);
  int result = process_execute(file);
  sema_up(&file_sema);
  return result;
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
  sema_down(&file_sema);
  bool result = filesys_remove(file); 
  sema_up(&file_sema);
  return result;
}

int open (const char *file)
{
  int fd = -1;
  struct thread* cur = thread_current();
  sema_down(&file_sema);
  
  struct file* tempFile = filesys_open(file);
  if(tempFile != NULL)
  {
    fd = globalfd++;
    void* tempMapper = malloc(sizeof(struct file_mapper)); 
    struct file_mapper *mapper = (struct file_mapper*)tempMapper;
    mapper->name = file;
    mapper->fd = fd;
    mapper->open_file = tempFile;  
    list_push_back(&(cur->file_list), &(mapper->elem));
  }
  
  sema_up(&file_sema);     
  return fd; 
}

int filesize (int fd)
{
  int result = 0;
  sema_down(&file_sema);
  struct file* f = mapFile(fd);
  if(f != NULL)
  {
    result = file_length(f);
  }
  sema_up(&file_sema);
  return result;
}

int read (int fd, void *buffer, unsigned length)
{
  if(fd == 0)
  {
    unsigned i;
    for(i = 0; i < length; i++) {
      char c = (char)input_getc();
      memcpy(buffer, &c, 1);
    } 
    return length;
  }

  int result = -1; 
  sema_down(&file_sema);
  struct file* f = mapFile(fd);
  if(f != NULL)
  {
    result = file_read(f, *(char**)buffer, length);
  }
  sema_up(&file_sema);
  return result;
}

int write (int fd, const void *buffer, unsigned length)
{
  if(fd == 1) 
  {
    putbuf(*(char**)buffer, length);
    return length;
  }
  
  sema_down(&file_sema);
  struct file* writeFile = mapFile(fd);
  int bytesWritten = 0; 
  if(writeFile != NULL)
  {
    bytesWritten = file_write(writeFile, *(char**)buffer, length);    
  }
  sema_up(&file_sema);
  return bytesWritten;
}

void seek (int fd, unsigned position)
{
  sema_down(&file_sema);
  struct file* f = mapFile(fd);
  if(f != NULL)
  {
    file_seek(f, position);
  }
  sema_up(&file_sema);  
}

unsigned tell (int fd)
{
  unsigned result = 0;
  sema_down(&file_sema);
  struct file* f = mapFile(fd);
  if(f != NULL)
  {
    result = file_tell(f);
  }
  sema_up(&file_sema);
  return result;
}

void close (int fd)
{
  sema_down(&file_sema);
  struct file* file = mapFile(fd);
  if(file != NULL)
  { 
    const char* file_name = get_file_name(fd);
    file_close(file);
    close_all_fd(file_name);
  }
  sema_up(&file_sema);
}

/*
 * Returns the file that corresponds with the file descriptor fd,
 * or null if there isn't and open file with that descriptor. 
 */
struct file* mapFile(int fd) 
{

  struct list_elem *e;

  for(e = list_begin (&thread_current()->file_list); 
      e != list_end (&thread_current()->file_list);
      e = list_next (e))
  {
    struct file_mapper *temp = list_entry (e, struct file_mapper, elem);
    if(temp->fd == fd)
    {
      return temp->open_file;
    }  
  } 

  return NULL;   
}

const char* get_file_name(int fd) {
  struct list_elem *e;

  for(e = list_begin (&thread_current()->file_list);
      e != list_end (&thread_current()->file_list);
      e = list_next (e))
  {
    struct file_mapper *temp = list_entry (e, struct file_mapper, elem);
    if(temp->fd == fd)
    {
      return temp->name;
    }
  }

  return NULL;
}

void close_all_fd(const char* file_name)
{
  struct list_elem *e;

  for(e = list_begin (&thread_current()->file_list);
      e != list_end (&thread_current()->file_list);
      e = list_next (e))
  {
    struct file_mapper *temp = list_entry (e, struct file_mapper, elem);
    if(strcmp(temp->name, file_name) == 0)
    {
      e = list_remove(e);
      e = list_prev(e);
      free(temp);
    }
  }
}
