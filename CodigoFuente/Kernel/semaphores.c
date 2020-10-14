#include "semaphores.h"

#define MAXSEM 10

typedef struct sem_list {
    sem_t* sem;
    struct sem_list* next;
} sem_list;

static int size = 0;
static int sem_ids = 0;

sem_list* semaphore_list = NULL;

int sem_open(const char* name, int initial) {
    if (semaphore_list == NULL) {
        semaphore_list = my_malloc(sizeof(sem_list*));
        if (semaphore_list == NULL) return -1;
    }

    int exist = find_sem_by_name(name);
    if (exist)
        return 0;

    sem_t* sem = (sem_t*)my_malloc(sizeof(sem_t*));

    if (sem == NULL)
        return -1;

    add_sem_to_list(sem);

    sem->value = initial;
    sem->lock = 0;
    sem->chain = createQueue(initial);

    strcpy(sem->name, name);

    if (sem->chain == NULL)
        return -1;

    return 0;
}

int add_sem_to_list(sem_t* sem) {
    if (semaphore_list == NULL) return 0;
    if (semaphore_list != NULL && semaphore_list->next == NULL) {
        semaphore_list->sem = sem;
        return 1;
    }

    sem_list* iterator = semaphore_list;
    while (iterator->next != NULL)
        iterator = iterator->next;

    iterator->next = my_malloc(sizeof(sem_list*));
    iterator->next->sem = sem;
    drawWord(iterator->sem->name);
    return 1;
}

int find_sem_by_name(const char* name) {
    sem_list* sems = semaphore_list;

    while (sems != NULL) {
        drawWord(sems->sem->name);
        if (strcmp(sems->sem->name, name) == 0) {
            return 1;
        }
        sems = sems->next;
    }
    return 0;
}

void sem_close(const char* name) {
    sem_t* sem = find_sem_by_name(name);
    //Remove sem from list
    if (semaphore_list == NULL || sem == NULL) return;
    if (semaphore_list->sem == sem) semaphore_list = semaphore_list->next;

    sem_list* previousSem = semaphore_list;
    sem_list* currentSem = semaphore_list->next;
    int i;

    for (i = 0; i < size && currentSem->sem != sem; i++) {
        previousSem = currentSem;
        currentSem = currentSem->next;
    }
    if (currentSem->sem == sem) {
        previousSem->next = currentSem->next;
        my_free(sem->chain);
        my_free(sem);
        my_free(currentSem);
        size--;
        return 0;
    }
}

void sem_wait(const char* name) {
    sem_t* s = find_sem_by_name(name);
    if (s == NULL)
        return;
    acquire(&(s->lock));

    if (s->value > 0) {
        s->value--;
    } else {
        release(&(s->lock));
        sleep(s->chain);
        acquire(&(s->lock));
        s->value--;
    }
    release(&(s->lock));
}

void sem_post(const char* name) {
    sem_t* s = find_sem_by_name(name);
    if (s == NULL)
        return;
    acquire(&(s->lock));
    s->value++;
    wakeup(s->chain);
    release(&(s->lock));
}

void release(int* lock) {
    _xchg(lock, 0);
}

void sleep(Queue* p) {
    process* current = getreadyListProcess();
    enqueue(p, current);
    blockProcess(current->pid);
}

void acquire(int* lock) {
    while (_xchg(lock, 1) != 0)
        ;
}

void wakeup(Queue* p) {
    process* process = dequeue(p);
    blockProcess(process->pid);
}

void printSemaphoreWaitList(Queue* q) {
    char pidBuff[20];
    int i;
    for (i = 0; i < q->size; i++) {
        intToStr(q->array[i]->pid, pidBuff);
        drawWord(pidBuff);
        drawWord(" - ");
    }
}

void printSemaphoreState() {
    if (semaphore_list == NULL)
        return;
    sem_list* current = semaphore_list;
    int i;
    char semBuff[50];

    for (i = 0; i < size; i++) {
        drawWord(current->sem->name);
        drawWord(" - Lock:");
        intToStr(current->sem->lock, semBuff);
        drawWord(semBuff);
        drawWord(" - Value:");
        intToStr(current->sem->value, semBuff);
        drawWord(semBuff);
        drawWord(" - Blocked: ");
        printSemaphoreWaitList(current->sem->chain);
        jumpLine();
        current = current->next;
    }
}
