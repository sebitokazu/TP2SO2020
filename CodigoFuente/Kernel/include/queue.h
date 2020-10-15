#ifndef QUEQUE_H_
#include "mem_manager.h"
#include "process.h"
// A structure to represent a queue
typedef struct Queue {
    int size, front;
    unsigned int capacity;
    process **array;
} Queue;

Queue *createQueue(unsigned int capacity);
int isFull(Queue *queue);
int isEmpty(Queue *queue);
void enqueue(Queue *queue, process *item);
process *dequeue(Queue *queue);
void deleteQueue(Queue *q);
#endif