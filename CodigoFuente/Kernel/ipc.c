#include "pipe.h"
#include "semaphores.h"

typedef struct pipe_list {
    pipe* pipe;
    struct pipe_list* next;
} pipe_list;

static pipe_list* pipes = NULL;

int createPipe(const char* name, int pid) {
    pipe* p = getPipe(name);
    if (p != NULL) {
        p->pids[1] = pid;
    } else {
        p = (pipe*)my_malloc(sizeof(pipe));
        if (p == NULL) return -1;
        p->pipe_id = pipe_ids++;
        p->pids[0] = pid;
        p->pids[1] = -1;
        p->nwrite = 0;
        p->nread = 0;
        strcpy(p->name, name);

        pipe_list* node = (pipe_list*)my_malloc(sizeof(pipe_list));
        if (node == NULL)
            return -1;
        node->pipe = p;
        node->next = pipes;
        pipes = node;
        return 1;
    }
}
pipe* createShellPipe(const char* name, int pid) {
    pipe* p = getPipe(name);
    if (p != NULL) {
        p->pids[1] = pid;
    } else {
        p = (pipe*)my_malloc(sizeof(pipe));
        if (p == NULL) return NULL;
        p->pipe_id = pipe_ids++;
        p->pids[0] = pid;
        p->pids[1] = -1;
        p->nwrite = 0;
        p->nread = 0;
        strcpy(p->name, name);

        pipe_list* node = (pipe_list*)my_malloc(sizeof(pipe_list));
        if (node == NULL)
            return NULL;
        node->pipe = p;
        node->next = pipes;
        pipes = node;
    }
    return p;
}

void pipe_wakeup(pipe* p) {
    if (getCurrentPID() == p->pids[0])
        unblockProcess(p->pids[1]);
    else
        unblockProcess(p->pids[0]);
}

void pipe_sleep(uint64_t pid) {
    blockProcess(pid);
}

int writePipe(const char* name, char* str, int n) {
    pipe* p = getPipe(name);
    int i;
    for (i = 0; i < n && i < MAXBUFFERLENGTH; i++) {
        if (p->nwrite >= p->nread + MAXBUFFERLENGTH) {
            pipe_wakeup(p);
            pipe_sleep(getCurrentPID());
        }
        p->buffer[p->nwrite++ % MAXBUFFERLENGTH] = str[i];
        if (str[i] == '/0')
            break;
    }
    pipe_wakeup(p);

    return i;
}

int readPipe(const char* name, char* buf, int n) {
    pipe* p = getPipe(name);
    int i;
    for (i = 0; i < n && i < MAXBUFFERLENGTH; i++) {
        if (p->nread >= p->nwrite) {
            pipe_wakeup(p);
            pipe_sleep(getCurrentPID());
        }
        buf[i] = p->buffer[p->nread++ % MAXBUFFERLENGTH];
        if (buf[i] == '/0')
            break;
    }
    pipe_wakeup(p);
    return i;
}

void closePipe(const char* name, int pid) {
    pipe* p = getPipe(name);
    if (p->pids[0] == pid) {
        p->pids[0] = -1;
    } else {
        p->pids[1] = -1;
    }
    if (p->pids[0] + p->pids[1] == -2)
        deletePipe(name);
}

void deletePipe(const char* name) {
    if (pipes == NULL)
        return;
    pipe_list* current = pipes;
    pipe_list* prev = NULL;

    while (strcmp(current->pipe->name, name) != 0) {
        if (current->next == NULL) {
            return NULL;
        } else {
            prev = current;
            current = current->next;
        }
    }
    if (current == pipes) {
        pipes = pipes->next;
    } else {
        prev->next = current->next;
    }
    my_free(current->pipe);
    my_free(current);
}

pipe* getPipe(const char* name) {
    pipe_list* aux = pipes;
    while (aux != NULL) {
        if (strcmp(aux->pipe->name, name) == 0)
            return aux->pipe;
        aux = aux->next;
    }
    return NULL;
}

void printPipes() {
    char buf[30];
    pipe_list* aux = pipes;
    while (aux != NULL) {
        drawWord(aux->pipe->name);
        drawWord(" - ID:");
        intToStr(aux->pipe->pipe_id, buf);
        drawWord(buf);
        drawWord(" - PID A:");
        intToStr(aux->pipe->pids[0], buf);
        drawWord(buf);
        drawWord(" - PID B:");
        intToStr(aux->pipe->pids[1], buf);
        drawWord(buf);
        jumpLine();
        aux = aux->next;
    }
}

#define MAXSEM 10

typedef struct sem_list {
    sem_t* sem;
    struct sem_list* next;
} sem_list;

sem_list* semaphore_list = NULL;
static int cant = 0;

void sem_sleep(LLQueue* p) {
    int current = getCurrentPID();
    enqueue(p, current);
    blockProcess(current);
}

void sem_wakeup(LLQueue* p) {
    if (!isEmpty(p)) {
        int pid = dequeue(p);
        unblockProcess(pid);
    }
}

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
    sem->chain = createLLQueue();

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
        sem_sleep(s->chain);
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
    sem_wakeup(s->chain);
    release(&(s->lock));

    yield();
}

void release(int* lock) {
    _xchg(lock, 0);
}

void acquire(int* lock) {
    while (_xchg(lock, 1) != 0)
        ;
}

void printSemaphoreWaitList(LLQueue* q) {
    char pidBuff[20];
    int i;
    QNode* node = q->front;
    while (node != NULL) {
        intToStr(node->key, pidBuff);
        drawWord(pidBuff);
        drawWord(" - ");
        node = node->next;
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