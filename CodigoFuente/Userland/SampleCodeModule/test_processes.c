#include <stdio.h>

#include "test_util.h"
#include "processlib.h"

//TO BE INCLUDED
void endless_loop() {
    while (1)
        ;
    my_exit();
}

uint32_t my_create_process(char* name) {
    char* argv[] = {"endless_loop &"};

    int res = exec(&endless_loop, 1, argv);
    return res;
}

uint32_t my_kill(uint32_t pid) {
    return kill(pid);
}

uint32_t my_block(uint32_t pid) {
    return 0;
}

uint32_t my_unblock(uint32_t pid) {
    return 0;
}

#define MAX_PROCESSES 3  //Should be around 80% of the the processes handled by the kernel

enum State { ERROR,
             BLOCKED,
             RUNNING,
             KILLED };

typedef struct P_rq {
    int pid;
    enum State state;
} p_rq;

void test_processes() {
    p_rq p_rqs[MAX_PROCESSES];
    uint8_t rq;
    uint8_t alive = 0;
    uint8_t action;

    while (1) {
        // Create MAX_PROCESSES processes
        for (rq = 0; rq < MAX_PROCESSES; rq++) {
            p_rqs[rq].pid = my_create_process("endless_loop");  // TODO: Port this call as required

            if (p_rqs[rq].pid == -1) {             // TODO: Port this as required
                printf("Error creating process");  // TODO: Port this as required
                printfd(p_rqs[rq - 1].pid);
                enter();
                checkMemoryStatus();
                my_exit();
            } else {
                p_rqs[rq].state = RUNNING;
                alive++;
            }
        }

        // Randomly kills, blocks or unblocks processes until every one has been killed
        while (alive > 0) {
            for (rq = 0; rq < MAX_PROCESSES; rq++) {
                action = GetUniform(2) % 2;

                switch (action) {
                    case 0:
                        if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
                            if (my_kill(p_rqs[rq].pid) == -1) {   // TODO: Port this as required
                                printf("Error killing process");  // TODO: Port this as required
                                enter();
                                my_exit();
                            }
                            p_rqs[rq].state = KILLED;
                            alive--;
                        }
                        break;

                    case 1:
                        if (p_rqs[rq].state == RUNNING) {
                            if (block(p_rqs[rq].pid) == -1) {      // TODO: Port this as required
                                printf("Error blocking process");  // TODO: Port this as required
                                enter();
                                my_exit();
                            }
                            p_rqs[rq].state = BLOCKED;
                        }
                        break;
                }
            }

            // Randomly unblocks processes
            for (rq = 0; rq < MAX_PROCESSES; rq++)
                if (p_rqs[rq].state == BLOCKED && GetUniform(2) % 2) {
                    if (unblock(p_rqs[rq].pid) == -1) {      // TODO: Port this as required
                        printf("Error unblocking process");  // TODO: Port this as required
                        enter();
                        my_exit();
                    }
                    p_rqs[rq].state = RUNNING;
                }

            yield();
        }
    }
}
