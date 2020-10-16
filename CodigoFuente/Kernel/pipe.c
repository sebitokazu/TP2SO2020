#include "pipe.h"

#include "lib.h"

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

void wakeup(pipe* p) {
    if (getCurrentPID() == p->pids[0])
        unblockProcess(p->pids[1]);
    else
        unblockProcess(p->pids[0]);
}

void sleep(uint64_t pid) {
    blockProcess(pid);
}

int writePipe(const char* name, char* str, int n) {
    pipe* p = getPipe(name);
    int i;
    for (i = 0; i < n && i < MAXBUFFERLENGTH; i++) {
        if (p->nwrite >= p->nread + MAXBUFFERLENGTH) {
            wakeup(p);
            sleep(getCurrentPID());
        }
        p->buffer[p->nwrite++ % MAXBUFFERLENGTH] = str[i];
        if (str[i] == '/0')
            break;
    }
    wakeup(p);

    return i;
}

int readPipe(const char* name, char* buf, int n) {
    pipe* p = getPipe(name);
    int i;
    for (i = 0; i < n && i < MAXBUFFERLENGTH; i++) {
        if (p->nread >= p->nwrite) {
            wakeup(p);
            sleep(getCurrentPID());
        }
        buf[i] = p->buffer[p->nread++ % MAXBUFFERLENGTH];
        if (buf[i] == '/0')
            break;
    }
    wakeup(p);
    return i;
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
