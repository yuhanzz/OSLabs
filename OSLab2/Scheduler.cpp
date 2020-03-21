#include <iostream>
#include "Scheduler.h"

// FCFS

void FcfsScheduler::add_process(Process *process, int current_time)
{
    process->latest_enqueue_time = current_time;
    active_queue.push_back(process);
}

Process *FcfsScheduler::get_next_process(int current_time)
{
    if (active_queue.empty()) {
        return NULL;
    }
    Process *process = active_queue.front();
    active_queue.pop_front();

    process->cpu_waiting_time += current_time - process->latest_enqueue_time;

    return process;
}


