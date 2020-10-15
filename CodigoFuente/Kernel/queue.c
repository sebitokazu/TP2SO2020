#include "queue.h"

/*
    Implementation from Geeksforgeeks
    https://www.geeksforgeeks.org/queue-set-1introduction-and-array-implementation/
*/

// function to create a queue
// of given capacity.
// It initializes size of queue as 0
Queue *createQueue(unsigned int capacity) {
    struct Queue *queue = (struct Queue *)my_malloc(
        sizeof(Queue));
    queue->capacity = capacity;

    // This is important, see the enqueue
    queue->array = (process **)my_malloc(
        queue->capacity * sizeof(process *));
    return queue;
}

// Queue is full when size becomes
// equal to the capacity
int isFull(Queue *queue) {
    return (queue->size == queue->capacity);
}

// Queue is empty when size is 0
int isEmpty(Queue *queue) {
    return (queue->size == 0);
}

// Function to add an item to the queue.
// It changes rear and size
void enqueue(Queue *queue, process *item) {
    if (isFull(queue)) return;
    queue->array[queue->size] = item;
    queue->size = queue->size + 1;
}

// Function to remove an item from queue.
// It changes front and size
process *dequeue(Queue *queue) {
    if (isEmpty(queue))
        return NULL;
    process *item = queue->array[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

void deleteQueue(Queue *q) {
    my_free(q->array);
    my_free(q);
}

/*// Function to get front of queue
int front(struct Queue *queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->front];
}

// Function to get rear of queue
int rear(struct Queue *queue)
{
    if (isEmpty(queue))
        return INT_MIN;
    return queue->array[queue->rear];
}*/
