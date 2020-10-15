#include "semaphore_lib.h"

extern qword syscall(qword rdi, qword rsi, qword rdx, qword rcx, qword r8, qword r9);

int sem_open(const char* name, int size) {
    syscall(22, name, size, 0, 0, 0);
}

void sem_wait(const char* name) {
    syscall(24, name, 0, 0, 0, 0);
}

void sem_post(const char* name) {
    syscall(25, name, 0, 0, 0, 0);
}

void sem_close(const char* name) {
    syscall(23, name, 0, 0, 0, 0);
}

void print_sem() {
    syscall(26, 0, 0, 0, 0, 0);
}