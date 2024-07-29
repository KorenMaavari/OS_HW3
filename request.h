#ifndef __REQUEST_H__
#define __REQUEST_H__

#include "segel.h"
#include "threads_stats.h"

// Function to handle incoming requests
// Parameters:
//   fd - file descriptor for the client socket
//   arrival_time - timestamp when the request arrived
//   dispatch_time - timestamp when the request started being processed
//   stats - structure to hold statistics related to the threads
void requestHandle(int fd, struct timeval arrival_time, struct timeval dispatch_time, threads_stats stats, int* skipFd, struct timeval* skipDispatchTime);

#endif
