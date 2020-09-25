#include "applications.h"
#include "video_driver.h"

static int currentProgram = 1;
static void (*program1)(void);
static void (*program2)(void);

void loadProgram(int index, void(*ip)(void) ){
    switch(index){
        case 1: program1 = ip;
                break;
        case 2: program2 = ip;
                break;
    }
    return;
}

void switchApp(){
    if(currentProgram == 1){
        currentProgram = 2;
        program2();
    }
    else{
        currentProgram = 1;
        program1();
    }
}



void restartCurrentProgram(){
    if(currentProgram==1)
        program1();
    if(currentProgram==2)
        program2();
}

