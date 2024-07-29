#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H
#include "queue.h"
#include "segel.h"
#include <pthread.h>

//global variables
pthread_mutex_t queueMutex;
pthread_cond_t requestWaitingCond;
pthread_cond_t requestOverloadCond;
Queue waitingQueue;
Queue workingQueue;

#endif //GLOBALVARIABLES_H
