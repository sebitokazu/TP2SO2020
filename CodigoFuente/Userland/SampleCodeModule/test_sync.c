#include <stdint.h>

#include "processlib.h"
#include "semaphore_lib.h"
#include "stdlib.h"

#define TOTAL_PAIR_PROCESSES 2
#define SEM_ID "sem"

int64_t global;  //shared memory

void slowInc(int64_t *p, int64_t inc) {
    int64_t aux = *p;
    aux += inc;
    yield();
    *p = aux;
}

//int argc, char *argv[]
//uint64_t sem, uint64_t value, uint64_t N
void inc(int argc, char *argv[]) {
    uint64_t i;
    uint64_t sem = stringToInt(argv[1]);
    uint64_t value = stringToInt(argv[2]);
    uint64_t N = stringToInt(argv[3]);
    if (sem && !sem_open(SEM_ID, 1)) {
        printf("ERROR OPENING SEM");
        enter();
        my_exit();
    }
    for (i = 0; i < N; i++) {
        if (sem) sem_wait(SEM_ID);
        slowInc(&global, value);
        if (sem) sem_post(SEM_ID);
    }
    if (sem) sem_close(SEM_ID);

    printf("Final value: ");
    printfd(global);
    enter();

    my_exit();
}

void test_sync(int argc, char *argv[]) {
    uint64_t i;

    global = 0;

    printf("CREATING PROCESSES...(WITH SEM)");
    enter();

    char *argv1[] = {"incA &", "1", "1", "1000"};
    char *argv2[] = {"incB &", "1", "-1", "1000"};

    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        exec(&inc, 4, argv1);
        exec(&inc, 4, argv2);
    }

    my_exit();
}

void test_no_sync() {
    uint64_t i;

    global = 0;

    printf("CREATING PROCESSES...(WITHOUT SEM)\n");
    char **argv[] = {{"inc &", "0", "1", "1000000"}, {"inc &", "0", "-1", "1000000"}};
    for (i = 0; i < TOTAL_PAIR_PROCESSES; i++) {
        exec(&inc, 4, argv[i % 2]);
        exec(&inc, 4, argv[i % 2 + 1]);
    }
}
