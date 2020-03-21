#ifndef PROCESS_H_
#define PROCESS_H_

#include <iostream>

extern int maxprio;

typedef enum
{
    CREATED,
    READY,
    RUNNING,
    BLOCKED
} ProcessState;

class Process
{
public:
    // global for all processes
    static int available_pid;

    // static
    int pid;
    int static_priority;
    int arrive_time;
    int total_cpu;
    int cpu_burst_max;
    int io_burst_max;

    // dynamic
    int dynamic_priority;
    int remaining_cpu;
    int cpu_burst;
    int io_burst;
    int latest_trans_time;

    int finishing_time;
    int latest_enqueue_time;
    int cpu_waiting_time;
    int total_io_time;

    Process(int, int, int, int, int);
};

#endif