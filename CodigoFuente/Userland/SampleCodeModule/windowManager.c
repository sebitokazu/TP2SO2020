#include "windowManager.h"

extern void syscall(qword rdi, qword rsi, qword rdx, qword rcx, qword r8, qword r9);


void loadProgram(int index, void(*program)(void)){
    syscall((qword) 8,(qword) index,(qword) program,0,0,0);
}

void changeScreen(){
    syscall(4,0,0,0,0,0);
}

void enter(){
    syscall(5,0,0,0,0,0);
}

void backspace(){  
    syscall(6,0,0,0,0,0);
}
