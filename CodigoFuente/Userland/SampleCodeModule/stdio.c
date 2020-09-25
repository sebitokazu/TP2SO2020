#include "stdio.h"

extern void syscall(qword rdi, qword rsi, qword rdx, qword rcx, qword r8, qword r9);

char getChar(){
    char a;
    syscall(0,(qword) &a,0, 0, 0, 0);
    return a;
}

void scanf(char *dest, int size){
    if(size<=0) 
        return;
    syscall(1,(qword) dest,(qword) size,0,0,0);
}

void putChar(char c){
    syscall(2,(qword) c,0,0,0,0);
}

void printf(char *src){
    syscall(3,(qword) src,0,0,0,0);
}

//permite seleccionar el color
void putCharC(char c, int r, int g, int b){
    syscall(9,(qword) c,(qword) r,(qword) g,(qword) b,0);
}

//permite seleccionar el color
void printfC(char *src, int r, int g, int b){
    syscall(10,(qword) src,(qword) r,(qword) g,(qword) b,0);
}

void clearScreen(){
    syscall(7,0,0,0,0,0);
}


