#include "video_driver.h"
#include "font.h"
#include <interrupts.h>

#define WIDTH 1024
#define HEIGHT 768


struct vbe_mode_info_structure {
    uint16_t attributes;
    uint8_t window_a, window_b;
    uint16_t granularity;
    uint16_t window_size;
    uint16_t segment_a, segment_b;
    uint32_t win_func_ptr;
    uint16_t pitch;            // number of bytes per horizontal line
    uint16_t width, height;    // width and height in pixels
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
} __attribute__ ((packed));


struct colouredChar{
    char c;
    int r;
    int g;
    int b;
};

typedef struct vbe_mode_info_structure modeInfo;

modeInfo* screen_info = (modeInfo *)0x5C00;


//Pointers to screens
static int x1,x2,y1,y2, currentX, currentY;
static int currentScreen = 0; //0 if upper screen, 1 down screen
static int drawingChar=0;
static int twinkleOn = 0;

//Matrix representing char positions
static struct colouredChar firstScreen[41][113] = {};
static struct colouredChar secondScreen[41][113] = {};


void initializeVideoMode(){
    splitScreen();
    x1=0; y1=0;
    x2=0; y2=HEIGHT/2+1;
}

/*Returns pointer to framebuffer pointer to where to draw*/
char * getPixelDataByPosition(int x, int y){
    return (char*)(screen_info->framebuffer + (x+y*WIDTH) * 3);
}

/*Iterates between two windows*/
void changeActiveScreen(){
    while(twinkleOn){
        drawChar(0);
        deleteChar();
    }
    if(currentScreen == 0){
        x1 = currentX;
        y1 = currentY;
        currentScreen = 1;
        currentX = x2;
        currentY = y2;
    }
    else{
        x2 = currentX;
        y2 = currentY;
        currentScreen = 0;
        currentX = x1;
        currentY = y1;
    }
}

/*Jump*/
void jumpLine(){
    while(twinkleOn){
        drawChar(0);
        deleteChar();
    }
    if(currentY/9 == 40 || currentY/9 == 82) scrollScreen();
    else{
        currentY += 9;
        currentX = 0;
    }
}

/*Remove last character drawn*/
void deleteChar(){
    if(currentX == 0){
        if(currentY==0 || currentY==HEIGHT/2+1) return;
        currentY-=9;
        currentX=1017;
    }
    else currentX -=9;
    for(int col=0; col<8; col++){
        for(int row=0; row<8; row++){
            deletePixel((currentX)+row, (currentY)+col);
        }
    }
    saveChar(currentScreen, currentY/9, currentX/9, 0, 0, 0, 0);
}

/*Twinckle when waiting to write*/
void twinkle(){
    if(!drawingChar){
    if(!twinkleOn){
        drawChar('|');
        twinkleOn = 1;
    }
    else{
        deleteChar();
        twinkleOn = 0;
    }
    }
}

/*Remove twinkle, used when you draw and it hasn't been removed by itself*/
void removeTwinkle(){
    if(twinkleOn){
        deleteChar();
        twinkleOn--;
    }
}

/*Draw a word starting in the currentX and currentY position*/
void drawWord(char *string){
    int i=0;
    while(string[i] != 0){
        drawChar(string[i++]);
    }
}

/*Draw a word starting in the currentX and currentY position with rgb color*/
void drawColouredWord(char *string, int r, int g, int b){
    int i=0;
    while(string[i] != 0){
        drawColouredChar(string[i++], r, g, b);
    }
}

/*Draw a char starting in the currentX and currentY position*/
void drawChar(char c){
    if(validateWriteCoordenates(currentX,currentY)) scrollScreen();
    drawingChar=1;
    int row, col;
    removeTwinkle();
    saveChar(currentScreen, currentY/9, currentX/9, c, 255, 255, 255);
    for(col=0; col<8; col++){
        for(row=0; row<8; row++){
            if(font8x8_basic[c][col] & 1 << row){
                printPixel((currentX)+row, (currentY)+col);
            }
        }
    }
    currentX += 9;
    if(WIDTH-currentX < 9){
        currentY += 9;
        currentX = 0;
    }
    twinkleOn=0;
    drawingChar=0;
}

/*Draw a coloured char starting in the currentX and currentY position*/
void drawColouredChar(char c, int r, int g, int b){
    if(validateWriteCoordenates(currentX,currentY)) scrollScreen();
    drawingChar=1;
    int row, col;
    removeTwinkle();
    saveChar(currentScreen, currentY/9, currentX/9, c, r, g, b);
    for(col=0; col<8; col++){
        for(row=0; row<8; row++){
            if(font8x8_basic[c][col] & 1 << row){
                printColouredPixel((currentX)+row, (currentY)+col, r, g, b);
            }
        }
    }
    currentX += 9;
    if(WIDTH-currentX < 9){
        currentY += 9;
        currentX = 0;
    }
    twinkleOn=0;
    drawingChar=0;
}


void scrollScreen(){
    int i,j;
    struct colouredChar *c;
    if(!currentScreen){
    //delete all screen information
    for(i=0; i<WIDTH; i++)
        for(j=0; j<HEIGHT/2-1; j++)
            deletePixel(i,j);
    //Rewrite chars starting from second row
    currentX=0; currentY=0;
    for(i=1; i<41; i++)
        for(j=0; j<113; j++){
            c = &(firstScreen[i][j]);
            drawColouredChar(c->c, c->r, c->g, c->b);
        }
    //Delete chars from last row
    for(i=0;i<113;i++){
        c = &(firstScreen[40][i]);
        c->c = 0; c->r=0; c->g=0; c->b=0;
    }
        
    }
    else{
        //Same cycle for second screen
        for(i=0; i<WIDTH; i++)
            for(j=HEIGHT/2+1; j<HEIGHT; j++)
                deletePixel(i,j);
        currentX=0; currentY=HEIGHT/2+1;
        for(i=1; i<41; i++)
            for(j=0; j<113; j++){
                c = &(secondScreen[i][j]);
                drawColouredChar(c->c, c->r, c->g, c->b);
            }
        for(i=0;i<113;i++){
            c = &(secondScreen[40][i]);
            c->c = 0; c->r=0; c->g=0; c->b=0;
        }
    }
}

void clearScreen(){
    int i,j;
    struct colouredChar *c;
    if(!currentScreen){
        for(i=0; i<WIDTH; i++)
            for(j=0; j<HEIGHT/2-1; j++)
                deletePixel(i,j);
        for(i=0;i<41;i++)
            for(j=0;j<113;j++){
                c = &(firstScreen[i][j]);
                c->c = 0; c->r=0; c->g=0; c->b=0;
            }
        currentY=0;
    }
    else{
        for(i=0; i<WIDTH; i++)
            for(j=HEIGHT/2+1; j<HEIGHT; j++)
                deletePixel(i,j);
        for(i=0;i<41;i++)
        for(j=0;j<113;j++){
            c = &(secondScreen[i][j]);
            c->c = 0; c->r=0; c->g=0; c->b=0;
        }
        currentY=HEIGHT/2+1;
        }
    currentX=0;
}

int validateWriteCoordenates(int x,int y){
    if(y/9 == 40 || y/9 == 82){
        if(x/9 == 112)
            return 1;
    }
    return 0;
}

/*Print a pixel in an (x,y) coordenates*/
void printPixel(int x, int y){
    char *pos = getPixelDataByPosition(x,y);
    pos[0]=244;
    pos[1]=244;
    pos[2]=244;
}

/*Print a pixel in an (x,y) coordenates using rgb colors*/
void printColouredPixel(int x, int y, int r, int g, int b){
    char *pos = getPixelDataByPosition(x,y);
    pos[0] = b;
    pos[1] = g;
    pos[2] = r;
}

/*Delete a pixel in an (x,y) coordenates*/
void deletePixel(int x, int y){
    char *pos = getPixelDataByPosition(x,y);
    pos[0]=0;
    pos[1]=0;
    pos[2]=0;
}

/*Splits screen in two*/
void splitScreen(){
    for(int j=0; j<WIDTH; j++)
        printColouredPixel(j,HEIGHT/2,0,244,0);
}

/*Save char in char matrix*/
void saveChar(int screen, int row, int col, char c, int r, int g, int b){
    struct colouredChar *toSave;
    if(screen)
        toSave = &(secondScreen[row-42][col]);
    else
        toSave = &(firstScreen[row][col]);
    
    toSave->c = c;  toSave->r = r;  toSave->g = g;  toSave->b = b;
    return;
}

/*Draw a word in screen*/
void writeOnScreen(char *string){
    drawWord(string);
}

