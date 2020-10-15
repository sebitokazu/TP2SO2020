#include "semaphores.h"

#define MAXSEM 10

typedef struct sem_list {
    sem_t* sem;
    struct sem_list* next;
} sem_list;

sem_list* semaphore_list = NULL;
static int cant = 0;

int sem_open(const char* name, int initial) {
    sem_list* exist = find_sem_by_name(name);

    if (exist != NULL) {
        return 1;
    }
    sem_t* sem = (sem_t*)my_malloc(sizeof(sem_t*));

    if (sem == NULL)
        return 0;

    add_sem_to_list(sem);

    sem->value = initial;
    sem->lock = 0;
    sem->chain = createQueue(initial);

    strcpy(sem->name, name);

    if (sem->chain == NULL)
        return 0;

    return 1;
}

int add_sem_to_list(sem_t* sem) {
    sem_list* new_sem = my_malloc(sizeof(sem_list*));

    new_sem->sem = sem;
    new_sem->next = semaphore_list;

    semaphore_list = new_sem;

    return 1;
}

sem_t* find_sem_by_name(const char* name) {
    sem_list* sems = semaphore_list;

    while (sems != NULL) {
        if (strcmp(sems->sem->name, name) == 0) {
            return sems->sem;
        }
        sems = sems->next;
    }
    return NULL;
}

void sem_close(const char* name) {
    sem_t* sem = find_sem_by_name(name);
    //Remove sem from list
    if (semaphore_list == NULL || sem == NULL) return;

    sem_list* temp = semaphore_list;

    if (temp != NULL && temp->sem == sem) {
        semaphore_list = temp->next;
        deleteQueue(sem->chain);
        my_free(sem);
        my_free(temp);
        return;
    }
    sem_list* prev;
    while (temp != NULL && temp->sem != sem) {
        prev = temp;
        temp = temp->next;
    }

    prev->next = temp->next;

    deleteQueue(sem->chain);
    my_free(sem);
    my_free(temp);
    return;
}

void sem_wait(const char* name) {
    sem_t* s = find_sem_by_name(name);
    if (s == NULL) return;

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
    if (!isEmpty(p)) {
        process* process = dequeue(p);
        blockProcess(process->pid);
    }
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
    sem_list* current = semaphore_list;
    int i;
    char semBuff[50];

    while (current != NULL) {
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
