#include "segel.h"
#include "request.h"
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/wait.h>

// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// HW3: Parse the new arguments too
void getargs(int *port, int *numOfThreads, int *queueSize, char* SchedAlg, int argc, char *argv[])
{
    if (argc < 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    //arguments: [portnum] [threads] [queue_size] [schedalg]
    *port = atoi(argv[1]);
    *numOfThreads = atoi(argv[2]);
    *queueSize = atoi(argv[3]);
    SchedAlg = argv[4];
}


int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen, numOfThreads, queueSize;
    struct sockaddr_in clientaddr;
    char* SchedAlg;

    getargs(&port, &numOfThreads, &queueSize, SchedAlg, argc, argv);

    // 
    // HW3: Create some threads...
    //
    pthread_t threads[numOfThreads];
    for (unsigned int i=0; i<numOfThreads; i++){
        pthread_create(&threads[i], NULL, ???, NULL);
    }
//    if(strcmp(SchedAlg, "block") == 0) {
//        for (unsigned int i=0; i<numOfThreads; i++){
//            pthread_create(&threads[i], NULL, blockHandler, NULL);
//        }
//    }
//    if(strcmp(SchedAlg, "dt") == 0) {
//        for (unsigned int i=0; i<numOfThreads; i++){
//            pthread_create(&threads[i], NULL, dropTailHandler, NULL);
//        }
//    }
//    if(strcmp(SchedAlg, "dh") == 0) {
//        for (unsigned int i=0; i<numOfThreads; i++){
//            pthread_create(&threads[i], NULL, dropHeadHandler, NULL);
//        }
//    }
//    if(strcmp(SchedAlg, "bf") == 0) {
//        for (unsigned int i=0; i<numOfThreads; i++){
//            pthread_create(&threads[i], NULL, blockFlushHandler, NULL);
//        }
//    }
//    if(strcmp(SchedAlg, "random") == 0) {
//
//    }


    listenfd = Open_listenfd(port);
    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

	// 
	// HW3: In general, don't handle the request in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work. 
	// 
	requestHandle(connfd);

	Close(connfd);
    }

}


    


 
