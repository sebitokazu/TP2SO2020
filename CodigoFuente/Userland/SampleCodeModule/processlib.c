#include "processlib.h"
extern qword syscall(qword rdi, qword rsi, qword rdx, qword rcx, qword r8, qword r9);

void exec(void* entry_point, int argc, char* argv[]) {
    syscall(11, entry_point, argc, argv, 0, 0);
}

void checkMemoryStatus() {
    syscall(18, 0, 0, 0, 0, 0);
}

void ps() {
    syscall(15, 0, 0, 0, 0, 0);
}

void kill(qword pid) {
    syscall(16, pid, 0, 0, 0, 0);
}

qword getPID() {
    return syscall(19, 0, 0, 0, 0, 0);
}

void block(qword pid) {
    syscall(20, pid, 0, 0, 0, 0);
}

void my_exit() {
    syscall(12, 0, 0, 0, 0, 0);
}

void yield() {
    syscall(21, 0, 0, 0, 0, 0);
}

void nice(qword pid, qword priority) {
    syscall(17, pid, priority, 0, 0, 0);
}
void pipe_exec(void* entry_point1, char* argv1[], void* entry_point2, char* argv2[]) {
    syscall(26, entry_point1, argv1, entry_point2, argv2, 0);
}

void unblock(qword pid) {
    syscall(27, pid, 0, 0, 0, 0);
}