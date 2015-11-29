#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>

void syscall_init (void);
bool check_valid_pointer (void* esp);

#endif /* userprog/syscall.h */
