#include "process.h"

#include "lib.h"
#include "mem_manager.h"

#define STACK_SIZE 4096

extern uint64_t getCurrentPID();
static uint64_t pids = 0;

extern void forceTimerTick();

typedef struct stack_frame {
    //General use registers
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rbp;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;

    //iretq registers
    uint64_t rip;
    uint64_t cs;
    uint64_t eflags;
    uint64_t rsp;
    uint64_t ss;
    uint64_t base;

} stack_frame;

void *createStackFrame(void *stack_base, void *entry_point, int argc, char *argv[]);
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

void printStackFrame(uint64_t *rsp) {
    int i;
    char aux[65];
    for (i = 0; i < 21; i++) {
        intToStr(rsp[i], aux);
        drawWord(aux);
        jumpLine();
    }
}

process *createProcess(void *entry_point, int argc, char *argv[], int pipe_role) {
    process *new_process = (process *)my_malloc(sizeof(process *));
    void *stack_base = my_malloc(STACK_SIZE);

    if (new_process == NULL || stack_base == NULL)
        return NULL;

    new_process->rsp = createStackFrame(stack_base, entry_point, argc, argv);
    //printStackFrame(new_process->rsp);
    new_process->pid = pids++;
    new_process->state = READY;
    new_process->stack_base = stack_base;

    if (strcmp(argv[0], "init") != 0 && (strcmp(argv[0], "shell") != 0))
        new_process->parent_pid = getCurrentPID();
    else
        new_process->parent_pid = NULL;
    if (argv != NULL) {
        if (isBackgroundProcess(argv[0]))
            new_process->background = BACKGROUND;
        else
            new_process->background = FOREGROUND;
        strcpy(new_process->name, argv[0]);
    }
    switch (pipe_role) {
        case NONE:
            new_process->stdin = NULL;
            new_process->stdout = NULL;
            break;
        case STDIN:
            new_process->stdin = getPipe(SHELLPIPE);
            new_process->stdout = NULL;
            break;
        case STDOUT:
            new_process->stdout = getPipe(SHELLPIPE);
            new_process->stdin = NULL;
            break;
        default:
            return -1;
            break;
    }

    return new_process;
}

int isBackgroundProcess(char *name) {
    int length = my_strlen(name);
    return name[length - 1] == '&';
}

void freeProcess(process *process) {
    my_free(process->stack_base);  //deberia estar apuntando a stack_base no?
    my_free(process);
}

void *createStackFrame(void *stack_base, void *entry_point, int argc, char *argv[]) {
    stack_frame *stack_frame = ((char *)stack_base + STACK_SIZE - sizeof(stack_frame));
    stack_frame->r15 = 0;
    stack_frame->r14 = 1;
    stack_frame->r13 = 2;
    stack_frame->r12 = 3;
    stack_frame->r11 = 4;
    stack_frame->r10 = 5;
    stack_frame->r9 = 6;
    stack_frame->r8 = 7;
    stack_frame->rsi = argv;  //argv
    stack_frame->rdi = argc;  //argc
    stack_frame->rbp = 10;
    stack_frame->rdx = 11;
    stack_frame->rcx = 12;
    stack_frame->rbx = 13;
    stack_frame->rax = 14;

    stack_frame->rip = entry_point;
    stack_frame->cs = 0x8;
    stack_frame->eflags = 0x202;
    stack_frame->rsp = (char *)stack_base + STACK_SIZE - 1;  //?????
    stack_frame->ss = 0x0;
    stack_frame->base = stack_base;
    return ((char *)stack_base + STACK_SIZE - sizeof(stack_frame));
}

void yield() {
    forceTimerTick();
}