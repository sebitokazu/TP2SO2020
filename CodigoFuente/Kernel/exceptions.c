#include "applications.h"
#include "video_driver.h"

#define ZERO_EXCEPTION_ID 0
#define INVALID_OPCODE_EXCEPTION_ID 6

static void zero_division(void *rsp);
static void invalid_opcode(void *rsp);
static void printRegisters(qword* rsp);
static int intToStr(int x, char str[]);
static void reverse(char* str, int len);


void exceptionDispatcher(int exception, void *rsp) {
	if(exception == ZERO_EXCEPTION_ID)
		zero_division(rsp);
	if(exception == INVALID_OPCODE_EXCEPTION_ID)
		invalid_opcode(rsp);
}

static void zero_division(void *rsp) {
	drawWord("Do you know Maths? - zero division exception");
	jumpLine();
    drawWord("Register state when the exception happened: ");
    jumpLine();
    printRegisters((qword*)rsp);
	restartCurrentProgram();
}

static void invalid_opcode(void *rsp){
	drawWord("Try again - invalid opcode exception");
	jumpLine();
    drawWord("Register state when the exception happened: ");
    jumpLine();
    printRegisters((qword*)rsp);
	restartCurrentProgram();
}


static void printRegisters(qword *rsp){
    char aux[65];
    char names[20][4]={"rip", "rax", "rbx", "rcx", "rdx", "rbp", "rdi", "rsi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
    for(int i=0; i<16; i++){
        intToStr(rsp[15-i], aux);
        drawWord(names[i]);
        drawWord(": ");
        drawWord(aux);
        jumpLine();
    }
}


static int intToStr(int x, char str[])
{
    int i = 0;
    if(x != 0){
    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }
    reverse(str, i);
    str[i] = '\0';
    }
    else{
        str[0] = '0';
        str[1] = '\0';
        return 1;
    }
    return i;
}

static void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}
