#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
#include <list.h>
#include "filesys/file.h"

/*Holds info on files opened. Stored in a thread's file_list. */ 
struct file_mapper
{
  int fd;
  struct file* open_file;
  const char* name;
  bool deny_write;
  struct list_elem elem;
};

void syscall_init (void);
bool check_valid_pointer (const void* esp);
void check_valid_buffer(void* esp, int offset);
void check_valid_args(void* esp, int numArgs);

void halt (void);
void exit (int status);
int exec (const char *file);
int wait (int childProc);
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int open (const char *file);
int filesize (int fd);
int read (int fd, void *buffer, unsigned length);
int write (int fd, const void *buffer, unsigned length);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);

struct file_mapper* mapFile(int fd);
void close_all_fd(const char* file_name);
void close_all_files(void);
void remove_all_children(void);
void remove_child_on_wait(int tid);
int get_child_status(int tid);
void release_file_lock(void);

#endif /* userprog/syscall.h */
