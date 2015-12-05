#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
#include <list.h>
#include "filesys/file.h"

struct file_mapper
{
  int fd;
  struct file* file;
  struct list_elem elem;
};

void syscall_init (void);
bool check_valid_pointer (const void* esp);
void check_valid_buffer(void* esp, int offset);

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

struct file* mapFile(int fd);

#endif /* userprog/syscall.h */
