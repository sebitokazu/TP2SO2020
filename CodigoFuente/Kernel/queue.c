/*
    Linked List Queue Implementation
    From: https://www.geeksforgeeks.org/queue-linked-list-implementation/
*/
#include "queue.h"

// A utility function to create a new linked list node.
QNode* newNode(int k) {
    QNode* temp = (QNode*)my_malloc(sizeof(QNode));
    temp->key = k;
    temp->next = NULL;
    return temp;
}

// A utility function to create an empty queue
LLQueue* createLLQueue() {
    LLQueue* q = (LLQueue*)my_malloc(sizeof(LLQueue));
    q->front = q->rear = NULL;
    return q;
}

// The function to add a key k to q
void enqueue(LLQueue* q, int k) {
    // Create a new LL node
    QNode* temp = newNode(k);

    // If queue is empty, then new node is front and rear both
    if (q->rear == NULL) {
        q->front = q->rear = temp;
        return;
    }

    // Add the new node at the end of queue and change rear
    q->rear->next = temp;
    q->rear = temp;
}

// Function to remove a key from given queue q
int dequeue(LLQueue* q) {
    // If queue is empty, return NULL.
    if (q->front == NULL)
        return -1;

    // Store previous front and move front one node ahead
    struct QNode* temp = q->front;

    q->front = q->front->next;

    // If front becomes NULL, then change rear also as NULL
    if (q->front == NULL)
        q->rear = NULL;

    int pid = temp->key;
    my_free(temp);

    return pid;
}

int isEmpty(LLQueue* q) {
    return q->front == NULL;
}