#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
#include <list.h>
#include "filesys/file.h"

struct file_mapper
{
  int fd;
  struct file* open_file;
  const char* name;
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

struct file* mapFile(int fd);
const char* get_file_name(int fd);
void close_all_fd(const char* file_name);

#endif /* userprog/syscall.h */
