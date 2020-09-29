#include "malloc.h"

extern void syscall(qword rdi, qword rsi, qword rdx, qword rcx, qword r8, qword r9);

void* malloc(size_t size){
    syscall((qword) 13, (qword) size, 0, 0, 0, 0);
}

void free(void* ptr){
    syscall( (qword) 14, (qword) ptr, 0, 0, 0, 0);
}