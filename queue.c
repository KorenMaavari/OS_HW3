#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "queue.h"

// Define a constant for failure return value
const int FAIL = -1;

void initializeQueue(Queue* queue) {
    queue->size = 0;
    queue->tail = NULL;
    queue->head = NULL;
}

int push(Queue* queue, int fd, struct timeval arrival_time) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->fd = fd;
    newNode->next = queue->tail; // Set the next pointer to the current tail
    newNode->previous = NULL; // Set the previous pointer to NULL
    newNode->arrival_time = arrival_time;

    if (queue->size == 0) {
        // If the queue is empty, set both head and tail to the new node
        queue->head = newNode;
        queue->tail = newNode;
    } else {
        // Otherwise, update the current tail's previous pointer and set the new node as the tail
        queue->tail->previous = newNode;
        queue->tail = newNode;
    }

    queue->size++;
    return newNode->fd; // Return the added fd
}

int pop(Queue* queue) {
    if (queue->size == 0) {
        return FAIL; // Return FAIL if the queue is empty
    }

    Node* toDelete = queue->head; // Get the node to delete (head)
    int fd = queue->head->fd; // Get the fd of the head node

    queue->head = queue->head->previous; // Update the head to the previous node

    if (queue->size == 1) {
        queue->tail = NULL; // If it was the only element, set tail to NULL
    } else {
        queue->head->next = NULL; // Otherwise, update the new head's next pointer
    }

    free(toDelete);
    queue->size--;
    return fd;
}

int pop_by_fd(Queue* queue, int val) {
    Node* curr = queue->tail; // Start from the tail
    Node* toDelete = NULL;

    // Traverse the queue to find the node with the given fd
    while (curr) {
        if (curr->fd == val) {
            toDelete = curr;
            break;
        }
        curr = curr->next;
    }

    if (curr == NULL) {
        return FAIL; // Return FAIL if the fd is not found
    }

    int fd = toDelete->fd;

    // Update pointers based on the node's position
    if (queue->size == 1) {
        queue->tail = NULL;
        queue->head = NULL;
    } else if (toDelete->fd == queue->head->fd) {
        queue->head = queue->head->previous;
        queue->head->next = NULL;
    } else if (toDelete->fd == queue->tail->fd) {
        queue->tail = queue->tail->next;
        queue->tail->previous = NULL;
    } else {
        toDelete->previous->next = toDelete->next;
        toDelete->next->previous = toDelete->previous;
    }

    free(toDelete);
    queue->size--;
    return fd;
}

int pop_by_index(Queue* queue, int index) {
    // Assuming the index is valid
    Node* toDelete = queue->tail; // Start from the tail

    // Traverse to the node at the given index
    for (int i = 0; i < index; i++) {
        toDelete = toDelete->next;
    }

    int fd = toDelete->fd;

    // Update pointers based on the node's position
    if (queue->size == 1) {
        queue->tail = NULL;
        queue->head = NULL;
    } else if (toDelete->fd == queue->head->fd) {
        queue->head = queue->head->previous;
        queue->head->next = NULL;
    } else if (toDelete->fd == queue->tail->fd) {
        queue->tail = queue->tail->next;
        queue->tail->previous = NULL;
    } else {
        toDelete->previous->next = toDelete->next;
        toDelete->next->previous = toDelete->previous;
    }

    free(toDelete);
    queue->size--;
    return fd;
}

int getSize(Queue* queue) {
    return queue->size;
}
struct timeval getArrivalTime(Queue* queue) {
    return queue->head->arrival_time;
}

//void deleteQueue(Queue* queue) {
//    struct timeval arrival_time;
//    int size = queue->size;
//
//    // Pop all elements from the queue
//    for (int i = 0; i < size; ++i) {
//        pop(&queue, &fd, &arrival_time);
//    }
//}
