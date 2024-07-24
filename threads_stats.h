#ifndef __THREADS_STATS_H__
#define __THREADS_STATS_H__

// Definition of the Threads_stats structure
typedef struct Threads_stats {
    int id; // Thread ID
    int stat_req; // Number of static requests handled by the thread
    int dynm_req; // Number of dynamic requests handled by the thread
    int total_req; // Total number of requests handled by the thread
} *threads_stats;

// Initialize the statistics for a thread with a given ID
void initStats(threads_stats stats, int* id);

// Get the ID of the thread
int getId(threads_stats stats);

// Get the total number of requests handled by the thread
int getTotalReq(threads_stats stats);

// Increment the total number of requests handled by the thread
void incTotalReq(threads_stats stats);

// Get the number of static requests handled by the thread
int getStaticReq(threads_stats stats);

// Increment the number of static requests handled by the thread
void incStaticReq(threads_stats stats);

// Get the number of dynamic requests handled by the thread
int getDynamicReq(threads_stats stats);

// Increment the number of dynamic requests handled by the thread
void incDynamicReq(threads_stats stats);

#endif // __THREADS_STATS_H__
