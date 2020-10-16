#ifndef PIPE_H_
#include "types.h"

#define MAXBUFFERLENGTH 240
#define MAXNAME 50

#define SHELLPIPE "cmd_"
static int pipe_ids = 0;

typedef struct pipe {
    int pipe_id;
    int pids[2];
    char name[MAXNAME];
    char buffer[MAXBUFFERLENGTH];
    unsigned int nread;
    unsigned int nwrite;
} pipe;

int createPipe(const char* name, int pid);
int readPipe(const char* name, char* buf, int n);
int writePipe(const char* name, char* str, int n);
void deletePipe(const char* name);
pipe* getPipe(const char* name);

void printPipes();

#endif