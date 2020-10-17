#ifndef PIPE_LIB_H
#include "types.h"
int createPipe(const char* name);
int writePipe(const char* name, char* str, int n);
int readPipe(const char* name, char* buf, int n);
void printPipes();
void closePipe(const char* name);
#endif