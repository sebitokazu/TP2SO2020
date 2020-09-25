
#ifndef applications_h
#define applications_h

#include <stdio.h>



void loadProgram(int index, void(*ip)(void));
void switchApp();
void restartCurrentProgram();

#endif /* applications_h */