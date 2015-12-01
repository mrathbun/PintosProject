#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
#include <list.h>
#include "filesys/file.h"

void syscall_init (void);
bool check_valid_pointer (void* esp);

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

struct file_mapper
{
  int fd;
  struct file* file;
  struct list_elem elem;
};

#endif /* userprog/syscall.h */
