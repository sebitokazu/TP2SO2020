/* sampleCodeModule.c */
#include "calculator.h"
#include "shell.h"
#include "windowManager.h"

int main() {
    loadProgram(1,&initCalculator);
    loadProgram(2,&initShell);
    initCalculator();
    return 0;
}


