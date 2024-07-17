#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "queue.h"

// Define a constant for failure return value
const int FAIL = -1;

// Node structure for the queue
typedef struct Node {
    struct timeval arrival_time; // Time when the node was added
    int value;
    struct Node* previous;
    struct Node* next;
} Node;

typedef struct queue {
    int size; // Current size of the queue
    int max_size; // Maximum allowed size of the queue
    Node* tail;
    Node* head;
} queue;

queue* initializeQueue(int max_size) {
    queue* newQueue = (queue*)malloc(sizeof(queue));
    newQueue->size = 0;
    newQueue->tail = NULL;
    newQueue->head = NULL;
    newQueue->max_size = max_size;
    return newQueue;
}

int push(queue* queue, int value, struct timeval arrival_time) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->value = value;
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
    return newNode->value; // Return the added value
}

int pop(queue* queue, struct timeval *arrival_time_ptr) {
    if (queue->size == 0) {
        return FAIL; // Return FAIL if the queue is empty
    }

    Node* toDelete = queue->head; // Get the node to delete (head)
    int value = queue->head->value; // Get the value of the head node
    *arrival_time_ptr = queue->head->arrival_time; // Copy the arrival time

    queue->head = queue->head->previous; // Update the head to the previous node

    if (queue->size == 1) {
        queue->tail = NULL; // If it was the only element, set tail to NULL
    } else {
        queue->head->next = NULL; // Otherwise, update the new head's next pointer
    }

    free(toDelete);
    queue->size--;
    return value;
}

int pop_by_val(queue* queue, int val) {
    Node* curr = queue->tail; // Start from the tail
    Node* toDelete = NULL;

    // Traverse the queue to find the node with the given value
    while (curr) {
        if (curr->value == val) {
            toDelete = curr;
            break;
        }
        curr = curr->next;
    }

    if (curr == NULL) {
        return FAIL; // Return FAIL if the value is not found
    }

    int value = toDelete->value;

    // Update pointers based on the node's position
    if (queue->size == 1) {
        queue->tail = NULL;
        queue->head = NULL;
    } else if (toDelete->value == queue->head->value) {
        queue->head = queue->head->previous;
        queue->head->next = NULL;
    } else if (toDelete->value == queue->tail->value) {
        queue->tail = queue->tail->next;
        queue->tail->previous = NULL;
    } else {
        toDelete->previous->next = toDelete->next;
        toDelete->next->previous = toDelete->previous;
    }

    free(toDelete);
    queue->size--;
    return value;
}

int pop_by_index(queue* queue, int index) {
    // Assuming the index is valid
    Node* toDelete = queue->tail; // Start from the tail

    // Traverse to the node at the given index
    for (int i = 0; i < index; i++) {
        toDelete = toDelete->next;
    }

    int value = toDelete->value;

    // Update pointers based on the node's position
    if (queue->size == 1) {
        queue->tail = NULL;
        queue->head = NULL;
    } else if (toDelete->value == queue->head->value) {
        queue->head = queue->head->previous;
        queue->head->next = NULL;
    } else if (toDelete->value == queue->tail->value) {
        queue->tail = queue->tail->next;
        queue->tail->previous = NULL;
    } else {
        toDelete->previous->next = toDelete->next;
        toDelete->next->previous = toDelete->previous;
    }

    free(toDelete);
    queue->size--;
    return value;
}

int getSize(queue* queue) {
    return queue->size;
}

void deleteQueue(queue* queue) {
    struct timeval arrival_time;
    int size = queue->size;

    // Pop all elements from the queue
    for (int i = 0; i < size; ++i) {
        pop(queue, &arrival_time);
    }
}
