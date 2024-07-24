#ifndef QUEUE_H_
#define QUEUE_H_

#include <sys/time.h>
#include <stdlib.h>
#include "segel.h"

// Node structure for the queue
typedef struct Node {
    struct timeval arrival_time; // Time when the node was added
    int fd;
    struct Node* previous;
    struct Node* next;
} Node;

typedef struct Queue {
    int size; // Current size of the queue
    Node* tail;
    Node* head;
} Queue;

void initializeQueue(Queue* queue);
int pop(Queue* queue);
int pop_by_fd(Queue* queue, int val);
int pop_by_index(Queue* queue, int index);
int push(Queue* queue, int fd, struct timeval arrival_time);
int getSize(Queue* queue);
struct timeval getArrivalTime(Queue* queue);

#endif /* QUEUE_H_ */
