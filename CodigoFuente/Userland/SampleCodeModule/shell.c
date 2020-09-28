#include "shell.h"

#include "processlib.h"
#include "stdlib.h"

static char commands[COMMANDS_QTY][COMMAND_MAX_LENGTH] = {"help", "exceptions", "inforeg", "printmem", "systime", "processor-temp", "processor-info", "clear", "mem", "ps", "loop", "kill", "nice", "block", "mypid"};
static char commands_description[COMMANDS_QTY][DESCRIPTION_MAX_LENGTH] = {HELP_DESC, EXCEPTIONS_DESC, INFOREG_DESC, PRINTMEM_DESC, SYSTIME_DESC, PROCESSOR_TEMP_DESC, PROCESSOR_INFO_DESC, CLEAR_DESC, MEM_DESC, PS_DESC, LOOP_DESC, KILL_DESC, NICE_DESC, BLOCK_DESC, MYPID_DESC};
char buffer[COMMAND_MAX_LENGTH] = {0};
static int i = 0, ctrl = 0, changedScreen = 0;

void initShell() {
    char c = 0;
    char command_arguments[MAX_ARGUMENTS_ACCEPTED + 1][COMMAND_MAX_LENGTH] = {{0}};

    if (!changedScreen) {
        printfC(PROMPT_MSG, 0, 255, 247);
    } else {
        changedScreen = 0;
    }

    while ((c = getChar()) != '\n' && (i < COMMAND_MAX_LENGTH - 1)) {
        switch (c) {
            case 0:
                break;
            case -1:
                ctrl = 0;
                break;
            case 1:
                ctrl = 1;
                break;
            case ' ':  //no permite mas de un espacio
                if (i > 0 && buffer[i - 1] != ' ') {
                    buffer[i++] = ' ';
                    putChar(c);
                }
                break;
            case '\b':
                if (i != 0) {
                    backspace();
                    buffer[--i] = 0;
                }
                break;
            case '\t':
                break;
            default:
                if (ctrl) {
                    if (c == 's') {
                        changedScreen = 1;
                        changeScreen();
                        return;
                    }
                    if (c == 'r')
                        saveRegisters();
                } else {
                    putChar(c);
                    buffer[i++] = c;
                }
        }
    }
    if (i == COMMAND_MAX_LENGTH - 1)
        printf(" We are not WSL2 :'( command or argument too long");
    else {
        buffer[i] = 0;

        int arg_qty = filterCommand(buffer, command_arguments);
        int pos = indexOf(command_arguments[0]);

        enter();

        if (pos == -1)
            printf(COMMAND_NOT_FOUND_MSG);
        else {
            int aux;
            switch (pos) {
                case 0:  //help
                    if (arg_qty != 0)
                        printf(INVALID_ARGUMENTS_MSG);
                    else
                        helpUser();
                    break;
                case 1:  //exceptions
                    if (arg_qty != 1)
                        printf(INVALID_ARGUMENTS_MSG);
                    else {
                        i = 0;  //antes de la excepcion!!!
                        handle_exception(command_arguments[1][0]);
                    }
                    break;
                case 2:  //inforef
                    if (arg_qty != 0)
                        printf(INVALID_ARGUMENTS_MSG);
                    else
                        printRegisters();
                    break;
                case 3:  //printmem
                    if (arg_qty != 1) {
                        printf(INVALID_ARGUMENTS_MSG);
                        break;
                    }
                    if ((aux = validateDir(command_arguments[1])) == -1)
                        printf(" No ha ingresado una direccion hexa de 32 bits");
                    else
                        printMem((void*)aux);
                    break;
                case 4:  //systime
                    if (arg_qty != 0)
                        printf(INVALID_ARGUMENTS_MSG);
                    else
                        printSystemTime();
                    break;
                case 5:  //processor-temp
                    if (arg_qty != 0)
                        printf(INVALID_ARGUMENTS_MSG);
                    else
                        getCpuTemperature();
                    break;
                case 6:  //processor-info
                    if (arg_qty != 0)
                        printf(INVALID_ARGUMENTS_MSG);
                    else
                        getinfoCPU();
                    break;
                case 7:  //clear
                    if (arg_qty != 0)
                        printf(INVALID_ARGUMENTS_MSG);
                    else
                        clearScreen();
                    break;
                case 8:  //mem
                    if (arg_qty != 0)
                        printf(INVALID_ARGUMENTS_MSG);
                    else
                        checkMemoryStatus();
                    break;
                case 9:  //ps
                    if (arg_qty != 0)
                        printf(INVALID_ARGUMENTS_MSG);
                    else
                        ps();
                    break;
                case 10:  //loop
                    if (arg_qty > 1)
                        printf(INVALID_ARGUMENTS_MSG);
                    else {  //ver lo de fg y bg
                        char* name[5] = {"loop"};
                        exec(&loop, 1, name);
                    }
                    break;
                case 11:  //kill
                    if (arg_qty != 1)
                        printf(INVALID_ARGUMENTS_MSG);
                    else {
                        int pid = (int)stringToDouble(command_arguments[1]);
                        kill(pid);
                    }
                    break;
                case 12:  //nice
                    if (arg_qty != 2)
                        printf(INVALID_ARGUMENTS_MSG);
                    else
                        //function - not implemented yet
                        break;
                case 13:  //block
                    if (arg_qty != 1)
                        printf(INVALID_ARGUMENTS_MSG);
                    else {
                        int pid = (int)stringToDouble(command_arguments[1]);
                        block(pid);
                    }
                    break;
                case 14:
                    if (arg_qty != 0)
                        printf(INVALID_ARGUMENTS_MSG);
                    else {
                        char aux[10];
                        qword pid = getPID();
                        intToStr(pid, aux);
                        printf("My PID is ");
                        printf(aux);
                    }
                    break;
            }
        }
    }
    i = 0;
    enter();
    initShell();  //Podriamos hacer un while(1) que abarque todo el codigo
                  //de initShell() para evitar la recursion infinita de funciones
}

/* inedxOf: Busca un string en un vector de strings.
Retorna la posicion donde lo encontro, o -1 si no lo encontro.
*/
static int indexOf(char* str) {
    int i;
    int found = 0;
    for (i = 0; i < COMMANDS_QTY && !found; i++)
        found = strcmp(commands[i], str);
    if (!found)
        i = 0;
    return i - 1;
}

/* filterCommand: Deja en la primer fila de la matriz el comando, 
en cada una de las siguientes fila deja los argumentos ingresados por el
usuario y retorna la cantidad de argumentos.
*/
static int filterCommand(char buffer[COMMAND_MAX_LENGTH], char com_args[MAX_ARGUMENTS_ACCEPTED + 1][COMMAND_MAX_LENGTH]) {
    int args = 0;  //con args=0 es el comando
    int i = 0, j = 0;
    char c;
    while ((c = buffer[i]) != 0 && args < MAX_ARGUMENTS_ACCEPTED + 1) {
        if (c == ' ') {  //recordar que no se aceptan espacios consecutivos
            args++;
            j = 0;
        } else
            com_args[args][j++] = c;
        i++;
    }
    if (j == 0)  //ultimo caracter era un espacio
        args--;
    return args;
}

/* helpUser: Imprime en pantalla los comandos con una breve
descripcion de lo que realizan.
*/
static void helpUser() {
    for (int i = 0; i < COMMANDS_QTY; i++) {
        printf(commands_description[i]);
        enter();
    }
}

void busy_wait(qword n) {
    qword i;
    for (i = 0; i < n; i++)
        ;
}

void loop() {
    char aux[10];
    qword pid = getPID();
    intToStr(pid, aux);
    while (1) {
        printf(aux);
        busy_wait(MINOR_WAIT);
    }
}

/*Inicio funciones cpu info*/
static void getinfoCPU() {
    printf(getCpuId1());  //en cpuid.asm
    enter();
    printf(getCpuId2());  //en cpuid.asm
    enter();

    char aux2[30];
    int aux = getCpuId3();  //en cpuid.asm
    int b = (aux & 0xF);
    intToStr(b, aux2);
    printf("Stepping ID: ");
    printf(aux2);
    b = (aux & 0xF0) >> 4;
    intToStr(b, aux2);
    printf(", Model: ");
    printf(aux2);
    b = (aux & 0xF00) >> 8;
    intToStr(b, aux2);
    printf(", Family ID: ");
    printf(aux2);
    b = (aux & 0x3000) >> 12;
    intToStr(b, aux2);
    printf(", Processor type: ");
    printf(aux2);
}

/*Fin funciones cpu info*/

/*Inicio funciones cpu temperature*/
static void getCpuTemperature() {
    int i1 = getCpuTempTarget();   // en cputemp.asm
    int i2 = getCpuThermStatus();  //en cputemp.asm
    char aux[20];
    int tt = (i1 & 0xFF0000) >> 16;  //23:16 Temperature Target
    int ts = (i2 & 0x7F0000) >> 16;  //22:16 Digital Readout

    intToStr(tt, aux);
    printf("Temperature target: ");
    printf(aux);
    enter();
    intToStr(ts, aux);
    printf("Therm status: ");
    printf(aux);
    enter();

    intToStr(tt - ts, aux);
    printf("CPU temperature: ");
    printf(aux);
}

/*Fin funciones cpu temperature*/

/*Inicio funciones exception*/
static void handle_exception(char c) {
    switch (c) {
        case '0':
            zero_exception();
            break;
        case '1':
            invalid_opcode_exception();
            break;  //en ud2_invalid_opcode.asm
        default:
            printf(INVALID_ARGUMENTS_MSG);
            break;
    }
}

static int zero_exception() {
    int i = 3 / 0;
    return i;
}
/*Fin funciones exception*/

/*Inicio funciones para obtener la hora del sistema*/
int getRTCHours() {
    int hours = getHours();  //en rtc_asm.asm
    return toDec(hours);
}

int getRTCMinutes() {
    int minutes = getMinutes();  //en rtc_asm.asm
    return toDec(minutes);
}

int getRTCSeconds() {
    int seconds = getSeconds();  //en rtc_asm.asm
    return toDec(seconds);
}

int toDec(int num) {
    return ((num & 0xF0) >> 4) * 10 + (num & 0x0F);
}

void printSystemTime() {
    char aux[8];
    int a = localHour(getRTCHours());
    a = intToStr(a, aux);
    printf(aux);
    printf(" : ");
    a = intToStr(getRTCMinutes(), aux);
    printf(aux);
    printf(" : ");
    a = intToStr(getRTCSeconds(), aux);
    printf(aux);
}

int localHour(int hour) {
    switch (hour) {
        case 0:
            return 21;
        case 1:
            return 22;
        case 2:
            return 23;

        default:
            return hour - 3;
    }
}
/*Fin funciones para obtener la hora del sistema*/

/*Inicio funciones inforeg*/

static void printRegisters() {
    char aux[65];
    char names[20][4] = {"rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};
    for (int i = 0; i < 16; i++) {
        intToStr(registers[i], aux);
        printf(names[i]);
        printf(": ");
        printf(aux);
        enter();
    }
}

static void saveRegisters() {
    registers = getRegisters();  //en inforeg_user.asm
}

/*Fin funciones inforeg*/

/*Inicio funciones printmem*/

void printMem(void* dir) {
    char* aux1 = dir;
    char aux2[10];
    for (int i = 0; i < 32; i++) {
        decimalToHex((unsigned int)aux1[i], aux2);
        printf(aux2);
        printf(" ");
        if ((i + 1) % 4 == 0)
            enter();
    }
}

/*decimalToHex: Recibe un numero en base 10 y lo transforma a string hexadecimal. */
static void decimalToHex(unsigned int decimal, char* ans) {
    int i = 0, j, n = decimal;
    char digit = 0;
    char aux[10];

    if (decimal > 127) {
        ans[0] = '0';
        ans[1] = '0';
        ans[2] = 0;
        return;
    }

    if (n == 0)
        aux[i++] = '0';

    while (decimal != 0) {
        digit = decimal % 16;
        aux[i++] = decimalCharToHex(digit);  //los pone al reves
        decimal = decimal / 16;
    }
    if (n < 16)
        aux[i++] = '0';
    i--;
    for (j = 0; i >= 0; j++, i--)
        ans[j] = aux[i];
    ans[j] = 0;
}

/*decimalCharToHex: Transforma un caracter decimal a hexa. */
static char decimalCharToHex(char c) {
    if (c >= 0 && c <= 9)
        return c + 48;
    return c + 55;
}

/*validateDir: retorna la direccion hexa de 32 bits en decimal, y -1 si era invalida. */
int validateDir(char* argument) {
    int error = 0, i = 0, base = 1, aux;
    unsigned int decimalDir = 0;
    char c;
    if (strlen(argument) > 8)
        error = 1;

    while (argument[i] != 0)
        i++;
    i--;  //indice del bit menos significativo

    for (; i >= 0 && !error; i--) {
        c = argument[i];
        aux = hexCharToInt(c);
        if (aux == -1)
            error = 1;
        decimalDir += base * aux;
        base *= 16;
    }

    if (error)
        decimalDir = -1;
    return decimalDir;
}

/*hexCharToInt: Transforma un caracter en hexa a decimal, retorna -1 si no era hexa*/
int hexCharToInt(char c) {
    if (c > 47 && c < 58)
        return c - 48;
    switch (c) {
        case 'a':
        case 'A':
            return 10;
        case 'b':
        case 'B':
            return 11;
        case 'c':
        case 'C':
            return 12;
        case 'd':
        case 'D':
            return 13;
        case 'e':
        case 'E':
            return 14;
        case 'f':
        case 'F':
            return 15;
        default:
            return -1;
    }
}
/*Fin funciones printmem*/
