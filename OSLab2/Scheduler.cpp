#include <iostream>
#include "Scheduler.h"

Process *BaseScheduler::get_next_process(int current_time)
{
    if (active_queue.empty()) {
        return NULL;
    }
    Process *process = active_queue.front();
    active_queue.pop_front();

    process->cpu_waiting_time += current_time - process->latest_enqueue_time;

    return process;
}

// FCFS

void FcfsScheduler::add_process(Process *process, int current_time)
{
    process->latest_enqueue_time = current_time;
    active_queue.push_back(process);
}


// LCFS
void LcfsScheduler::add_process(Process *process, int current_time)
{
    process->latest_enqueue_time = current_time;
    active_queue.push_front(process);
}

// SRTF
void SrtfScheduler::add_process(Process *process, int current_time)
{
    process->latest_enqueue_time = current_time;

        if (active_queue.empty())
    {
        active_queue.push_front(process);
        return;
    }

    std::list<Process *>::iterator iterator;

    int current_remaining = process->remaining_cpu;
    for (iterator = active_queue.begin(); iterator != active_queue.end(); iterator++)
    {
        int compared_remaining = (*iterator)->remaining_cpu;
        if (compared_remaining > current_remaining)
        {
            active_queue.insert(iterator, process);
            return;
        }
    }
    active_queue.push_back(process);
}