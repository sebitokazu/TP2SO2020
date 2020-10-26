#include "syscalls.h"

#include "idtLoader.h"
#include "interrupts.h"
#include "keyBoardDriver.h"
#include "mem_manager.h"
#include "scheduler.h"
#include "video_driver.h"
#include "semaphores.h"

extern void _hlt();
extern void *getIP();
static void syscallReadChar(char *dest);
static void syscallReadChars(char *dest, int size);
static void syscallDrawChar(char c);
static void syscallDrawChars(char *src);
static void syscallJumpline();

long syscall_handler(qword rdi, qword rsi, qword rdx, qword rcx, qword r8, qword r9) {
    switch (rdi) {
        case 0:  //readChar - getChar
            syscallReadChar((char *)rsi);
            break;
        case 1:  //readChars - scanf
            syscallReadChars((char *)rsi, (int)rdx);
            break;
        case 2:  //drawChar - putChar
            syscallDrawChar((char)rsi);
            break;
        case 3:  //drawWord - printf
            syscallDrawChars((char *)rsi);
            break;
        case 4:
            //changeActiveScreen();
            //switchApp();
            break;
        case 5:  //enter()
            syscallJumpline();
            break;
        case 6:  //backspace()
            deleteChar();
            break;
        case 7:
            clearScreen();
            break;
        case 8:  //loadProgram
            //loadProgram((int)rsi, (void (*)(void))rdx);
            break;
        case 9:
            drawColouredChar((char)rsi, (int)rdx, (int)rcx, (int)r8);
            break;
        case 10:
            drawColouredWord((char *)rsi, (int)rdx, (int)rcx, (int)r8);
            break;
        case 11:
            return initProcess((void *)rsi, (int)rdx, (char **)rcx);
        case 12:
            my_exit();
            break;
        case 13:
            return my_malloc((size_t)rsi);
        case 14:
            my_free((void *)rsi);
            break;
        case 15:
            printProcesses();
            break;
        case 16:
            return removeProcess(rsi);
        case 17:
            return changePriority(rsi, rdx);
            break;
        case 18:
            checkMemoryStatus();
            break;
        case 19:
            return getCurrentPID();
        case 20:
            return blockProcess(rsi);
        case 21:
            yield();
            break;
        case 22:
            return createPipe(rsi, getCurrentPID());
            break;
        case 23:
            return writePipe((const char *)rsi, (char *)rdx, (int)rcx);
        case 24:
            return readPipe((const char *)rsi, (char *)rdx, (int)rcx);
        case 25:
            printPipes();
            break;
        case 26:
            return initPipedProcesses((void *)rsi, (char **)rdx, (void *)rcx, (char **)r8);
        case 27:
            return unblockProcess(rsi);
        case 28:
            closePipe((const char *)rsi, getCurrentPID());
            break;
        case 29:
            return sem_open((const char *)rsi, rdx);
            break;
        case 30:
            sem_close((const char *)rsi);
            break;
        case 31:
            sem_wait((const char *)rsi);
            break;
        case 32:
            sem_post((const char *)rsi);
            break;
        case 33:
            printSemaphoreState();
            break;
        default:
            break;
    }
    return 0;
}

void setUp_syscalls() {
    setup_IDT_entry(0x80, (uint64_t)&_irq80Handler);
}

static void syscallJumpline() {
    process *current = getCurrentProcess();
    if (current->stdout_p == NULL)
        jumpLine();
    else {
        char *c = "\n";
        writePipe(current->stdin_p->name, c, 1);
    }
}

static void syscallReadChar(char *dest) {
    process *current = getCurrentProcess();
    if (current->stdin_p == NULL) {
        while (emptyBuffer()) {
            _hlt();
        }
        readChar(dest);
    } else {
        readPipe(current->stdin_p->name, dest, 1);
    }
}

static void syscallReadChars(char *dest, int size) {
    process *current = getCurrentProcess();
    if (current->stdin_p == NULL) {
        int i = 0;
        char aux[5];
        while (size > 0) {
            syscallReadChar(aux);
            if (aux[0] != 0) {
                dest[i++] = aux[0];
                size--;
            }
        }
    } else {
        readPipe(current->stdin_p == NULL, dest, size);
    }
}

static void syscallDrawChar(char c) {
    process *current = getCurrentProcess();
    if (current->stdout_p == NULL)
        drawChar(c);
    else
        writePipe(current->stdout_p->name, &c, 1);
}

static void syscallDrawChars(char *src) {
    process *current = getCurrentProcess();
    if (current->stdout_p == NULL)
        drawWord(src);
    else {
        writePipe(current->stdout_p->name, src, my_strlen(src));
    }
}