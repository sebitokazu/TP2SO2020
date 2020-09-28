#include "video_driver.h"

#include <interrupts.h>

#include "font.h"
#include "lib.h"

#define WIDTH 1024
#define HEIGHT 768

struct vbe_mode_info_structure {
    uint16_t attributes;
    uint8_t window_a, window_b;
    uint16_t granularity;
    uint16_t window_size;
    uint16_t segment_a, segment_b;
    uint32_t win_func_ptr;
    uint16_t pitch;          // number of bytes per horizontal line
    uint16_t width, height;  // width and height in pixels
    uint8_t w_char, y_char, planes, bpp, banks;
    uint8_t memory_model, bank_size, image_pages;
    uint8_t reserved0;

    uint8_t red_mask, red_position;
    uint8_t green_mask, green_position;
    uint8_t blue_mask, blue_position;
    uint8_t reserved_mask, reserved_position;
    uint8_t direct_color_attributes;

    uint32_t framebuffer;
    uint32_t off_screen_mem_off;
    uint16_t off_screen_mem_size;
    uint8_t reserved1[206];
} __attribute__((packed));

struct colouredChar {
    char c;
    int r;
    int g;
    int b;
};

typedef struct vbe_mode_info_structure modeInfo;

modeInfo *screen_info = (modeInfo *)0x5C00;

//Pointers to screens
static int currentX, currentY;
static int currentScreen = 0;  //0 if upper screen, 1 down screen
static int drawingChar = 0;
static int twinkleOn = 0;

//Matrix representing char positions
static struct colouredChar firstScreen[41][113] = {};
static struct colouredChar secondScreen[41][113] = {};

static struct colouredChar mappedScreen[82][113] = {};

void initializeVideoMode() {
    currentX = 0;
    currentY = 0;
}

/*Returns pointer to framebuffer pointer to where to draw*/
char *getPixelDataByPosition(int x, int y) {
    return (char *)(screen_info->framebuffer + (x + y * WIDTH) * 3);
}

/*Jump*/
void jumpLine() {
    while (twinkleOn) {
        drawChar(0);
        deleteChar();
    }
    if (currentY / 9 >= 81)  //82
        scrollScreen();      //Aca
    else {
        currentY += 9;
        currentX = 0;
    }
}

/*Remove last character drawn*/
void deleteChar() {
    if (currentX == 0) {
        if (currentY == 0) return;
        currentY -= 9;
        currentX = 1017;
    } else
        currentX -= 9;
    for (int col = 0; col < 8; col++) {
        for (int row = 0; row < 8; row++) {
            deletePixel((currentX) + row, (currentY) + col);
        }
    }
    saveChar(currentY / 9, currentX / 9, 0, 0, 0, 0);
}

/*Twinckle when waiting to write*/
void twinkle() {
    if (!drawingChar) {
        if (!twinkleOn) {
            drawChar('|');
            twinkleOn = 1;
        } else {
            deleteChar();
            twinkleOn = 0;
        }
    }
}

/*Remove twinkle, used when you draw and it hasn't been removed by itself*/
void removeTwinkle() {
    if (twinkleOn) {
        deleteChar();
        twinkleOn--;
    }
}

/*Draw a word starting in the currentX and currentY position*/
void drawWord(char *string) {
    int i = 0;
    while (string[i] != 0) {
        drawChar(string[i++]);
    }
}

/*Draw a word starting in the currentX and currentY position with rgb color*/
void drawColouredWord(char *string, int r, int g, int b) {
    int i = 0;
    while (string[i] != 0) {
        drawColouredChar(string[i++], r, g, b);
    }
}

/*Draw a char starting in the currentX and currentY position*/
void drawChar(char c) {
    if (validateWriteCoordenates(currentX, currentY)) scrollScreen();
    drawingChar = 1;
    int row, col;
    removeTwinkle();
    saveChar(currentY / 9, currentX / 9, c, 255, 255, 255);
    for (col = 0; col < 8; col++) {
        for (row = 0; row < 8; row++) {
            if (font8x8_basic[c][col] & 1 << row) {
                printPixel((currentX) + row, (currentY) + col);
            }
        }
    }
    currentX += 9;
    if (WIDTH - currentX < 9) {
        currentY += 9;
        currentX = 0;
    }
    twinkleOn = 0;
    drawingChar = 0;
}

/*Draw a coloured char starting in the currentX and currentY position*/
void drawColouredChar(char c, int r, int g, int b) {
    if (validateWriteCoordenates(currentX, currentY)) scrollScreen();
    drawingChar = 1;
    int row, col;
    removeTwinkle();
    saveChar(currentY / 9, currentX / 9, c, r, g, b);
    for (col = 0; col < 8; col++) {
        for (row = 0; row < 8; row++) {
            if (font8x8_basic[c][col] & 1 << row) {
                printColouredPixel((currentX) + row, (currentY) + col, r, g, b);
            }
        }
    }
    currentX += 9;
    if (WIDTH - currentX < 9) {
        currentY += 9;
        currentX = 0;
    }
    twinkleOn = 0;
    drawingChar = 0;
}

void scrollScreen() {
    /* int i,j;
    struct colouredChar *c;
    //delete all screen information
    for(i=0; i<WIDTH; i++)
        for(j=0; j<HEIGHT-1; j++)
            deletePixel(i,j);
    //Rewrite chars starting from second row
    currentX=0; currentY=0;
    for(i=1; i<83; i++)
        for(j=0; j<113; j++){
            c = &(mappedScreen[i][j]);
            drawColouredChar(c->c, c->r, c->g, c->b);
        }
    //Delete chars from last row
    for(i=0;i<113;i++){
        c = &(mappedScreen[80][i]);
        c->c = 0; c->r=0; c->g=0; c->b=0;
    }*/
    char *pos;
    char *next_pos;
    for (int i = 0; i < HEIGHT; i++) {
        pos = getPixelDataByPosition(0, 0 + i);
        next_pos = getPixelDataByPosition(0, 0 + (9) + i);
        memcpy(pos, next_pos, (screen_info->bpp) / 8 * (WIDTH));
    }
    currentX = 0;
    currentY = 81 * 9;
}

void clearScreen() {
    int i, j;
    struct colouredChar *c;
    for (i = 0; i < WIDTH; i++)
        for (j = 0; j < HEIGHT - 1; j++)
            deletePixel(i, j);
    for (i = 0; i < 82; i++)
        for (j = 0; j < 113; j++) {
            c = &(mappedScreen[i][j]);
            c->c = 0;
            c->r = 0;
            c->g = 0;
            c->b = 0;
        }
    currentY = 0;
    currentX = 0;
}

int validateWriteCoordenates(int x, int y) {
    if (y / 9 == 81) {
        if (x / 9 == 112)
            return 1;
    }
    return 0;
}

/*Print a pixel in an (x,y) coordenates*/
void printPixel(int x, int y) {
    char *pos = getPixelDataByPosition(x, y);
    pos[0] = 244;
    pos[1] = 244;
    pos[2] = 244;
}

/*Print a pixel in an (x,y) coordenates using rgb colors*/
void printColouredPixel(int x, int y, int r, int g, int b) {
    char *pos = getPixelDataByPosition(x, y);
    pos[0] = b;
    pos[1] = g;
    pos[2] = r;
}

/*Delete a pixel in an (x,y) coordenates*/
void deletePixel(int x, int y) {
    char *pos = getPixelDataByPosition(x, y);
    pos[0] = 0;
    pos[1] = 0;
    pos[2] = 0;
}

/*Save char in char matrix*/
void saveChar(int row, int col, char c, int r, int g, int b) {
    struct colouredChar *toSave;
    toSave = &(mappedScreen[row][col]);
    toSave->c = c;
    toSave->r = r;
    toSave->g = g;
    toSave->b = b;
    return;
}

/*Draw a word in screen*/
void writeOnScreen(char *string) {
    drawWord(string);
}
