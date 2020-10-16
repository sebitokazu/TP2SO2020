#include "pipe_lib.h"

extern qword syscall(qword rdi, qword rsi, qword rdx, qword rcx, qword r8, qword r9);

int createPipe(const char* name) {
    return syscall(22, name, 0, 0, 0, 0);
}

int writePipe(const char* name, char* str, int n) {
    return syscall(23, name, str, n, 0, 0);
}

int readPipe(const char* name, char* str, int n) {
    return syscall(24, name, str, n, 0, 0);
}

void printPipes() {
    syscall(25, 0, 0, 0, 0, 0);
}