#ifndef stdio_h
#define stdio_h

#include <types.h>

char getChar();

void scanf(char *dest, int size);

void printf(char *src);

void putChar(char c);

void printDouble(double d);

void clearScreen();

void putCharC(char c, int r, int g, int b);

void printfC(char *src, int r, int g, int b);

#endif /* stdio_h */
