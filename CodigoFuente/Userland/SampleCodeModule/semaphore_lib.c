#include "semaphore_lib.h"

extern qword syscall(qword rdi, qword rsi, qword rdx, qword rcx, qword r8, qword r9);

int sem_open(const char* name, int size) {
    return syscall(29, name, size, 0, 0, 0);
}

void sem_wait(const char* name) {
    syscall(31, name, 0, 0, 0, 0);
}

void sem_post(const char* name) {
    syscall(32, name, 0, 0, 0, 0);
}

void sem_close(const char* name) {
    syscall(30, name, 0, 0, 0, 0);
}

void print_sem() {
    syscall(33, 0, 0, 0, 0, 0);
}