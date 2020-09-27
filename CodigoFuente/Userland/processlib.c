#include "processlib.h"
extern void syscall(qword rdi, qword rsi, qword rdx, qword rcx, qword r8, qword r9);


void exec(void* entry_point, int argc, char* argv[]){
    syscall(11, entry_point, argc, argv, 0, 0);
}

void checkMemoryStatus(){
    syscall(18, 0, 0, 0, 0, 0);
}

void ps(){
    syscall(15,0,0,0,0,0);
}