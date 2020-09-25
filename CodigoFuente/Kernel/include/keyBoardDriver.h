#ifndef keyBoardDriver_h
#define keyBoardDriver_h

#include <stdio.h>

void keyboard_handler();

void readChar(char* dest);

void readChars(char *dest, int size);

int emptyBuffer();

int getBufferSize();

#endif /* keyBoardDriver_h */
