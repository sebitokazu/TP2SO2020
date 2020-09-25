#include "process.h"
#include "mem_manager.h"

#define STACK_SIZE 4096

static uint64_t pids = 0;

typedef struct stack_frame {

    //iretq registers
    uint64_t base;
    uint64_t rip;
    uint64_t cs;
    uint64_t eflags;
    uint64_t rsp;
    uint64_t ss;

    //General use registers
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
        

} stack_frame;

void* createStackFrame(void* stack_base, void* entry_point, int argc, char* argv[]);
/*
EL STACK TIENE QUE APUNTAR INICIALMENTE A UNA DIRECCION ALINEADA
Para un proceso nuevo:
ss=0x0
rsp=bp
rflags=0x202
cs=0x8
rip=main
Regs generales:
rdi=argc
rsi=argv
Sug: iniciarlos incrementalmente para debugging
*/

process* createProcess(void* entry_point, int argc, char* argv[]) {
    process* new_process = (process*)my_malloc(sizeof(process*));
    new_process->rsp = createStackFrame(my_malloc(STACK_SIZE), entry_point, argc, argv);
    new_process->pid = pids++;
    new_process->state = READY;

    return new_process;
}

void freeProcess(process* process) {
    my_free(process->rsp);  //deberia estar apuntando a stack_base no?
    my_free(process);
}

void* createStackFrame(void* stack_base, void* entry_point, int argc, char* argv[]) {
    stack_frame* stack_frame = (char*)stack_base;
    stack_frame->r15 = 0;
    stack_frame->r14 = 1;
    stack_frame->r13 = 2;
    stack_frame->r12 = 3;
    stack_frame->r11 = 4;
    stack_frame->r10 = 5;
    stack_frame->r9 = 6;
    stack_frame->r8 = 7;
    stack_frame->rsi = 8;  //argv
    stack_frame->rdi = 9;  //argc
    stack_frame->rbp = 10;
    stack_frame->rdx = 11;
    stack_frame->rcx = 12;
    stack_frame->rbx = 13;
    stack_frame->rax = 14;

    stack_frame->rip = entry_point;
    stack_frame->cs = 0x8;
    stack_frame->eflags = 0x202;
    stack_frame->rsp = stack_frame;  //?????
    stack_frame->ss = 0x0;
    stack_frame->base = stack_base;
    return stack_frame + sizeof(stack_frame) + 1;
}