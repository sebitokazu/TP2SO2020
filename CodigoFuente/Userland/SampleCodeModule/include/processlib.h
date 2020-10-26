#ifndef _PROCESSLIB_H
#include "types.h"

void checkMemoryStatus();
void ps();
int exec(void* entry_point, int argc, char* argv[]);
int kill(qword pid);
void my_exit();
qword getPID();
void yield();
void pipe_exec(void* entry_point1, char* argv1[], void* entry_point2, char* argv2[]);
int nice(qword pid, qword priority);
int unblock(qword pid);
int block(qword pid);

#endif