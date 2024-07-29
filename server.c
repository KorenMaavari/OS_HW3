#include "segel.h"
#include "request.h"
#include "queue.h"
#include "globalVariables.h"
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/wait.h>

#define BLOCK 1
#define DROP_TAIL 2
#define DROP_HEAD 3
#define BLOCK_FLUSH 4
#define DROP_RANDOM 5

// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

//global variables
int overloadPolicy;


// HW3: Parse the new arguments too
void getargs(int *port, int *numOfThreads, int *maxNumOfRequests, int argc, char *argv[])
{
    if (argc < 5) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    //arguments: [portnum] [threads] [queue_size] [schedalg]
    *port = atoi(argv[1]);
    *numOfThreads = atoi(argv[2]);
    *maxNumOfRequests = atoi(argv[3]);
    char* policy = argv[4];
    if(strcmp(policy, "block") == 0) {
        overloadPolicy = BLOCK;
    }
    if(strcmp(policy, "dt") == 0) {
        overloadPolicy = DROP_TAIL;
    }
    if(strcmp(policy, "dh") == 0) {
        overloadPolicy = DROP_HEAD;
    }
    if(strcmp(policy, "bf") == 0) {
        overloadPolicy = BLOCK_FLUSH;
    }
    if(strcmp(policy, "random") == 0) {
        overloadPolicy = DROP_RANDOM;
    }
}

void* workingThreadRoutine(void* args) {
    struct Threads_stats stats;
    initStats((threads_stats)&stats, (int*)args);    //Initialize Stats struct with args == thread index
    while(1) {
        pthread_mutex_lock(&queueMutex);
        while(getSize(&waitingQueue) == 0) {
            pthread_cond_wait(&requestWaitingCond, &queueMutex);
        }
        struct timeval start_time;
        struct timeval arrival_time = getHeadArrivalTime(&waitingQueue);
        int fd = pop(&waitingQueue); //Remove request from wait queue
        gettimeofday(&start_time, NULL);    //Save start time
        push(&workingQueue, fd, arrival_time);  //Add request for working queue
        pthread_mutex_unlock(&queueMutex);

        int skip_fd = -1;
        struct timeval skip_dispatch_time;
        struct timeval dispatch_time;
        timersub(&start_time, &arrival_time, &dispatch_time);
        requestHandle(fd, arrival_time, dispatch_time, &stats, &skip_fd, &skip_dispatch_time);
        Close(fd);
        if(skip_fd != -1) {
            struct timeval skip_arrival_time;
            getArrivalTimeByFd(&workingQueue, skip_fd, &skip_arrival_time);
            requestHandle(skip_fd, skip_arrival_time, skip_dispatch_time, &stats, NULL, NULL);
            Close(skip_fd);
        }

        pthread_mutex_lock(&queueMutex);
        pop_by_fd(&workingQueue, fd); //Remove request from work queue
        if(skip_fd != -1) {
            pop_by_fd(&workingQueue, skip_fd);
        }
        pthread_cond_signal(&requestOverloadCond);
//        if(getSize(&waitingQueue) + getSize(&workingQueue) == 0) {
//            pthread_cond_signal(&requestWaitingCond);
//        }
        pthread_cond_signal(&requestWaitingCond);
        pthread_mutex_unlock(&queueMutex);
    }
}

void handleBlock(struct timeval *arrival_time, int newFd) {
    pthread_mutex_lock(&queueMutex);
    pthread_cond_wait(&requestOverloadCond, &queueMutex);
    push(&waitingQueue, newFd, *arrival_time);
    pthread_cond_signal(&requestWaitingCond);
    pthread_mutex_unlock(&queueMutex);
}

void handleDropTail(struct timeval *arrival_time, int newFd) {
    pthread_mutex_lock(&queueMutex);
    Close(newFd);
    pthread_mutex_unlock(&queueMutex);
}

void handleDropHead(struct timeval *arrival_time, int newFd) {
    pthread_mutex_lock(&queueMutex);
    if(getSize(&waitingQueue) == 0) {
        Close(newFd);
    }
    else {
        int fdToClose = pop(&waitingQueue);
        Close(fdToClose);
        push(&waitingQueue, newFd, *arrival_time);
        pthread_cond_signal(&requestWaitingCond);
    }
    pthread_mutex_unlock(&queueMutex);
}

void handleBlockFlush(int fd) {
    pthread_mutex_lock(&queueMutex);
    while((getSize(&waitingQueue) + getSize(&workingQueue)) > 0) {
        pthread_cond_wait(&requestWaitingCond, &queueMutex);
    }
    Close(fd);
    pthread_mutex_unlock(&queueMutex);
}

void handleDropRandom(struct timeval *arrival_time, int newFd) {
    pthread_mutex_lock(&queueMutex);
    int range = (getSize(&waitingQueue) +1) / 2;
    for (int i = 0; i < range; ++i) {
        int index = rand() % getSize(&waitingQueue);
        int fdToClose = pop_by_index(&waitingQueue, index);
        Close(fdToClose);
    }
    if(range > 0) {
        push(&waitingQueue, newFd, *arrival_time);
        pthread_cond_signal(&requestWaitingCond);
    }
    else {  //If waiting queue is empty, no requests to drop, drop the new request
        Close(newFd);
    }
    pthread_mutex_unlock(&queueMutex);
}

int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;

    int numOfThreads, maxNumOfRequests;

    //Initialize mutex, condition variables and queues
    pthread_cond_init(&requestWaitingCond, NULL);
    pthread_cond_init(&requestOverloadCond, NULL);
    pthread_mutex_init(&queueMutex, NULL);
    initializeQueue(&waitingQueue);
    initializeQueue(&workingQueue);

    //Parse arguments from command line
    getargs(&port, &numOfThreads, &maxNumOfRequests, argc, argv);

    // 
    // HW3: Create some threads...
    //

    pthread_t threads[numOfThreads];
    for (unsigned int i=0; i<numOfThreads; i++){
        pthread_create(&threads[i], NULL, workingThreadRoutine, &i);
    }

    listenfd = Open_listenfd(port);
    struct timeval arrival_time;

    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *) &clientaddr, (socklen_t * ) & clientlen);
        gettimeofday(&arrival_time, NULL);

        if (getSize(&waitingQueue) + getSize(&workingQueue) < maxNumOfRequests) {
            //allowing to accept new request
            pthread_mutex_lock(&queueMutex);
            push(&waitingQueue, connfd, arrival_time);
            if (getSize(&workingQueue) < numOfThreads) {
                pthread_cond_signal(&requestWaitingCond);
            }
            pthread_mutex_unlock(&queueMutex);
        } else {
            switch (overloadPolicy) {
                case BLOCK:
                    handleBlock(&arrival_time, connfd);
                    break;
                case DROP_TAIL:
                    handleDropTail(&arrival_time, connfd);
                    break;
                case DROP_HEAD:
                    handleDropHead(&arrival_time, connfd);
                    break;
                case BLOCK_FLUSH:
                    handleBlockFlush(connfd);
                    break;
                case DROP_RANDOM:
                    handleDropRandom(&arrival_time, connfd);
                    break;
                default:
                    break;
            }
        }
    }
	// 
	// HW3: In general, don't handle the request in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work.
}


    


 
