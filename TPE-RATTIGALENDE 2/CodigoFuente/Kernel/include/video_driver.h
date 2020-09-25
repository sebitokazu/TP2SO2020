#ifndef video_driver_h
#define video_driver_h

#include <stdio.h>
#include <types.h>
#include <stdint.h>
#include <stdlib.h>


typedef struct vbe_mode_info_structure *modeInfoP;

int writePixel(int x, int y, int size);

void splitScreen();

void drawChar(char c);

char* getPosScreen();

void printChar(int x, int y, char c);

void printPixel(int x, int y);

void printColoredPixel(int x, int y, int r, int g, int b);

void drawWord(char *string);

void initializeVideoMode();

void changeActiveScreen();

void deleteChar();

void deletePixel(int x, int y);

void twinkle();

void removeTwinkle();

void saveChar(int screen, int row, int col, char c, int r, int g, int b);

int validateWriteCoordenates(int x,int y);

void scrollScreen();

void clearScreen();

void writeOnScreen(char *string);

void jumpLine();

void printColouredPixel(int x, int y, int r, int g, int b);

void drawColouredChar(char c, int r, int g, int b);

void drawColouredWord(char *string, int r, int g, int b);



#endif
