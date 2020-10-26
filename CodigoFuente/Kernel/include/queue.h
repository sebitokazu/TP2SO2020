#ifndef LLQUEUE_H_
#include "mem_manager.h"
// A linked list (LL) node to store a queue entry
typedef struct QNode {
    int key;
    struct QNode* next;
} QNode;

// The queue, front stores the front node of LL and rear stores the
// last node of LL
typedef struct LLQueue {
    QNode *front, *rear;
} LLQueue;

LLQueue* createLLQueue();
void deleteQueue(LLQueue* q);
void enqueue(LLQueue* q, int k);
int dequeue(LLQueue* q);
int isEmpty(LLQueue* q);
#endif