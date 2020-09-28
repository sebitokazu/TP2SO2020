#include "syscalls.h"

#include "applications.h"
#include "idtLoader.h"
#include "interrupts.h"
#include "keyBoardDriver.h"
#include "mem_manager.h"
#include "scheduler.h"
#include "video_driver.h"

extern void _hlt();
extern void *getIP();
static void syscallReadChar(char *dest);
static void syscallReadChars(char *dest, int size);
static void syscallDrawChar(char c);
static void syscallDrawChars(char *src);

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
            jumpLine();
            break;
        case 6:  //backspace()
            deleteChar();
            break;
        case 7:
            clearScreen();
            break;
        case 8:  //loadProgram
            loadProgram((int)rsi, (void (*)(void))rdx);
            break;
        case 9:
            drawColouredChar((char)rsi, (int)rdx, (int)rcx, (int)r8);
            break;
        case 10:
            drawColouredWord((char *)rsi, (int)rdx, (int)rcx, (int)r8);
            break;
        case 11:
            initProcess((void *)rsi, (int)rdx, (char **)rcx);
            break;
        case 12:
            //exit();
            break;
        case 13:
            return my_malloc((size_t)rsi);
            break;
        case 14:
            my_free((void *)rsi);
            break;
        case 15:
            printProcesses();
            break;
        case 16:
            removeProcess(rsi);
            break;
        case 17:
            //changePriority(pid,);
            break;
        case 18:
            checkMemoryStatus();
        default:
            break;
    }
    return 0;
}

void setUp_syscalls() {
    setup_IDT_entry(0x80, (uint64_t)&_irq80Handler);
}

static void syscallReadChar(char *dest) {
    while (emptyBuffer()) {
        _hlt();
    }
    readChar(dest);
}

static void syscallReadChars(char *dest, int size) {
    int i = 0;
    char aux[5];
    while (size > 0) {
        syscallReadChar(aux);
        if (aux[0] != 0) {
            dest[i++] = aux[0];
            size--;
        }
    }
}

static void syscallDrawChar(char c) {
    drawChar(c);
}

static void syscallDrawChars(char *src) {
    drawWord(src);
}