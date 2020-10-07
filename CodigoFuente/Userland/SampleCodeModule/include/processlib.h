#ifndef _PROCESSLIB_H
#include "types.h"

void checkMemoryStatus();
void ps();
void exec(void* entry_point, int argc, char* argv[]);
void kill(qword pid);
void my_exit();
qword getPID();
void yield();
#endif