#ifndef PROCESS_H_

#include <stdint.h>

#include "video_driver.h"
#include "pipe.h"

#define READY 0
#define BLOCKED 1
#define RUNNING 2
//#define WAITING 3
#define DEAD 4

#define FOREGROUND 0
#define BACKGROUND 1

#define MAX_NAME 50

#define NONE 0
#define STDIN 1
#define STDOUT 2

typedef struct process {
    uint64_t rsp;
    uint64_t stack_base;
    uint64_t pid;
    uint64_t parent_pid;
    uint8_t state;
    uint8_t background;
    pipe* stdin_p;
    pipe* stdout_p;
    char name[MAX_NAME];
} process;

process* createProcess(void* entry_point, int argc, char* argv[], int pipe_role);
void freeProcess(process* process);
int isBackgroundProcess(char *name);

#endif