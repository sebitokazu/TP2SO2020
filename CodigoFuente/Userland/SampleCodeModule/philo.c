/*
Adaptacion de la implementacion obtenida en:
https://www.geeksforgeeks.org/dining-philosopher-problem-using-semaphores/
*/

#include "philo.h"

int state[MAX_PHILOS];
static int phil_pids[MAX_PHILOS] = {-1};

int philos_number = N;
char S[MAX_PHILOS][5];

void sleep(int n) {
    long i;
    n *= 100000000;
    for (i = 0; i < n; i++)
        ;
}

void test(int phnum) {
    if (state[phnum] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        // state that eating
        state[phnum] = EATING;

        sleep(2);

        // sem_post(S[phnum]) has no effect
        // during takefork
        // used to wake up hungry philosophers
        // during putfork
        sem_post(S[phnum]);
    }
}

// take up chopsticks
void take_fork(int phnum) {
    sem_wait("mutex");

    // state that hungry
    state[phnum] = HUNGRY;

    // eat if neighbours are not eating
    test(phnum);

    sem_post("mutex");

    // if unable to eat wait to be signalled
    sem_wait(S[phnum]);

    sleep(1);
}

// put down chopsticks
void put_fork(int phnum) {
    sem_wait("mutex");

    // state that thinking
    state[phnum] = THINKING;

    test(LEFT);
    test(RIGHT);

    sem_post("mutex");
}

void philospher(int phnum) {
    while (1) {
        sleep(1);

        take_fork(phnum);

        print_table();

        sleep(0);

        put_fork(phnum);

        print_table();
    }
}

void new_philo(int i) {
    char aux[4];
    /* create semaphore */
    state[i] = THINKING;  //set philo's state
    itoa(i, aux);
    S[i][0] = 's';
    S[i][1] = aux[0];
    S[i][2] = aux[1];
    S[i][3] = aux[2];

    if (sem_open(S[i], 0) == -1)
        printf("ERROR WHILE OPENING SEM");

    /* create philosopher processes */
    char *name[] = {"p &"};
    itoa(i, aux);
    char *par[] = {aux};
    char *argv[] = {"p &", par};
    phil_pids[i] = exec(&philospher, i, argv);
}

void philo() {
    int i;
    char c;

    if (sem_open("mutex", 1) == -1)
        printf("ERROR WHILE OPENING SEM");

    for (i = 0; i < N; i++) {
        new_philo(i);
    }

    while ((c = getChar()) != 'q') {
        if (c == 'a' && philos_number < MAX_PHILOS) {
            sem_wait("mutex");
            new_philo(philos_number);
            philos_number++;
            sem_post("mutex");
        }

        if (c == 'r' && philos_number > 0) {
            sem_wait("mutex");
            philos_number--;
            sem_close(S[philos_number]);  //matar proceso
            kill(phil_pids[philos_number]);
            sem_post("mutex");
        }

        if (c == 't')
            print_table();
    }
    sem_wait("mutex");

    for (i = 0; i < philos_number; i++) {
        sem_close(S[i]);  //matar proceso
        kill(phil_pids[i]);
    }

    sem_post("mutex");
    sem_close("mutex");

    my_exit();
}

void print_table() {
    for (int i = 0; i < philos_number; i++) {
        if (state[i] == EATING)
            printf(" E ");
        else
            printf(" * ");
    }
    enter();
}