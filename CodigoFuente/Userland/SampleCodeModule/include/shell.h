#ifndef shell_h
#define shell_h

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "windowManager.h"

#define PROMPT_MSG "infocracks@shell-service: "
#define COMMAND_NOT_FOUND_MSG "Command not found :("
#define INVALID_ARGUMENTS_MSG "Invalid arguments!"
#define NOT_PIPEABLE_MSG "Some of the programs can't be piped"
#define HELP_DESC "help --> Funcion que despliega la lista de comandos y una breve descripcion de ellos."
#define EXCEPTIONS_DESC "exceptions --> Si el parametro es 0, ejecuta un programa que produce una excepcion de division por cero. Y si el parametro es 1 ejecuta otro que produce una del tipo invalid opcode. E imprime informacion sobre el error correspondiente."
#define INFOREG_DESC "inforeg --> Imprime la informacion que habia en los registros al momento de capturarlos. Para poder capturarlos presionar ctrl+r."
#define PRINTMEM_DESC "printmem --> Recibe como argumento una direccion de memoria y realiza un volcado de memoria de 32 bytes a partir de la direccion recibida."
#define SYSTIME_DESC "systime --> Impreme la hora del sistema."
#define PROCESSOR_TEMP_DESC "processor-temp --> Imprime la temperatura del procesador."
#define PROCESSOR_INFO_DESC "processor-info --> Imprime informacion sobre el procesador."
#define CLEAR_DESC "clear --> Borra todo el contenido de la pantalla."
#define MEM_DESC "mem --> Imprime el estado de la memoria."
#define PS_DESC "ps --> Imprime la lista de todos los procesos con sus propiedades: nombre, ID, prioridad, stack y base pointer."
#define LOOP_DESC "loop --> Imprime su ID con un saludo cada una determinada cantidad de segundos."
#define KILL_DESC "kill --> Mata un proceso dado su ID."
#define NICE_DESC "nice --> Cambia la prioridad de un proceso dado su ID y la nueva prioridad."
#define BLOCK_DESC "block --> Cambia el estado de un proceso entre bloqueado y listo dado su ID."
#define MYPID_DESC "mypid --> Imprime el PID de la shell."
#define TESTMM_DESC "testmm --> Inicia el programa de testeo del memory manager."
#define TESTSCH_DESC "testpro --> Inicia el programa de testeo del scheduler."
#define YIELD_DESC "yield --> Ejecuta una funcion para testear la renuncia al cpu."
#define PIPE_BAR_DESC "| --> Conecta dos procesos mediante un pipe. Ejemplo de uso: | ./p1 ./p2"
#define PRODUCER_DESC "producer --> Inicia el programa producer"
#define CONSUMER_DESC "consumer --> Inicia el programa consumer"
#define PIPES_DESC "pipes --> Imprime el estado de todos los pipes"
#define CAT_DESC "cat --> Imprime el stdin tal como lo recibe (no acepta backspace)"
#define UNBLOCK_DESC "unblock --> Cambia el estado de un proceso a listo dado su ID."
#define WC_DESC "wc --> Cuenta la cantidad de líneas del input."
#define FILTER_DESC "filter --> Filtra las vocales del input"
#define TESTSYNC_DESC "test_sync --> Inicia el programa de testeo de semaforos"
#define SEM_DESC "sem --> Imprime el estado de todos los semaforos"
#define PHILO_DESC "philo --> Implementa el problema de los filosofos comensales. Imprime el estado de la mesa. Con 'a' se agrega un filosofo, con 'r' se remueve uno y con 'q' se para el programa."
#define TESTPRIO_DESC "test_prio --> Inicia el programa de testeo de prioridades."
#define TESTNOSYNC_DESC "test_no_sync --> Inicia el programa de testeo sin semaforos"

#define COMMANDS_QTY 31
#define COMMAND_MAX_LENGTH 50
#define DESCRIPTION_MAX_LENGTH 300
#define MAX_ARGUMENTS_ACCEPTED 2
#define ARG_MAX_LENGTH 30

#define MINOR_WAIT 10000000

static int indexOf(char* str);
static int filterCommand(char buffer[COMMAND_MAX_LENGTH], char com_args[MAX_ARGUMENTS_ACCEPTED + 1][COMMAND_MAX_LENGTH]);

//Help function
static void helpUser();

//Exception
static void handle_exception(char c);
static int zero_exception();
extern void invalid_opcode_exception();

//System time functions
extern int getHours();
extern int getMinutes();
extern int getSeconds();
static int getRTCHours();
static int getRTCMinutes();
static int getRTCSeconds();
static int toDec(int num);
static int localHour(int hour);
static void printSystemTime();

//Processor id
extern char* getCpuId1();
extern char* getCpuId2();
extern int getCpuId3();
static void getinfoCPU();

//Processor temperature
extern int getCpuThermStatus();
extern int getCpuTempTarget();
static void getCpuTemperature();

//Inforeg
extern qword* getRegisters();
static void printRegisters();
static void saveRegisters();
static qword* registers;

//Printmem
static void printMem(void* dir);
static void decimalToHex(unsigned int decimal, char* ans);
static char decimalCharToHex(char c);
static int validateDir(char* argument);
static int hexCharToInt(char c);
extern void writeMem();  //funcion asm utilizada para verificar que printmem funciona correctamente

//Prod-cons
void producer();
void consumer();

void cat();
void wc();
void filter();
char isVocal(char c);

//Loop program
void loop();
void busy_wait(qword n);

void yieldTest();

void initShell();

#endif /* shell_h */
