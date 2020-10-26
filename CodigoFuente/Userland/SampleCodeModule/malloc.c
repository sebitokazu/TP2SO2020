#include "malloc.h"
#include "types.h"

extern qword syscall(qword rdi, qword rsi, qword rdx, qword rcx, qword r8, qword r9);

void* malloc(size_t size){
    return syscall((qword) 13, (qword) size, 0, 0, 0, 0);
}

void free(void* ptr){
    syscall( (qword) 14, (qword) ptr, 0, 0, 0, 0);
}