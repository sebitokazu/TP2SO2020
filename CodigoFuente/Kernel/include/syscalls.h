#ifndef syscalls_h
#define syscalls_h

#include <stdio.h>
#include <types.h>

long syscall_handler(qword rdi, qword rsi, qword rdx, qword rcx, qword r8, qword r9);

void setUp_syscalls();

#endif /* syscalls_h */
