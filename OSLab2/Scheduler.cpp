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
    if (active_queue.empty())
    {
        return NULL;
    }
    Process *process = active_queue.front();
    active_queue.pop_front();

    process->cpu_waiting_time += current_time - process->latest_enqueue_time;

    return process;
}

// LCFS
void LcfsScheduler::add_process(Process *process, int current_time)
{
    process->latest_enqueue_time = current_time;
    active_queue.push_front(process);
}

Process *LcfsScheduler::get_next_process(int current_time)
{
    if (active_queue.empty())
    {
        return NULL;
    }
    Process *process = active_queue.front();
    active_queue.pop_front();

    process->cpu_waiting_time += current_time - process->latest_enqueue_time;

    return process;
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

Process *SrtfScheduler::get_next_process(int current_time)
{
    if (active_queue.empty())
    {
        return NULL;
    }
    Process *process = active_queue.front();
    active_queue.pop_front();

    process->cpu_waiting_time += current_time - process->latest_enqueue_time;

    return process;
}

// RR
void RrScheduler::add_process(Process *process, int current_time)
{
    process->latest_enqueue_time = current_time;
    active_queue.push_back(process);
}

bool RrScheduler::test_preempt(Process *process, int quantum)
{
    if (process->cpu_burst > quantum)
    {
        return true;
    }
    return false;
}

Process *RrScheduler::get_next_process(int current_time)
{
    if (active_queue.empty())
    {
        return NULL;
    }
    Process *process = active_queue.front();
    active_queue.pop_front();

    process->cpu_waiting_time += current_time - process->latest_enqueue_time;

    return process;
}

// Prio
PrioScheduler::PrioScheduler(int maxprio)
{
    this->maxprio = maxprio;
    active_queue = new std::list<Process *>[maxprio];
    expired_queue = new std::list<Process *>[maxprio];
}

void PrioScheduler::add_process(Process *process, int current_time)
{
    process->latest_enqueue_time = current_time;

    int priority = process->dynamic_priority;
    if (priority == -1)
    {
        process->dynamic_priority = process->static_priority - 1;
        expired_queue[process->dynamic_priority].push_back(process);
    }
    else
    {
        active_queue[process->dynamic_priority].push_back(process);
    }
}

Process *PrioScheduler::get_next_process(int current_time)
{
    int highest_prio = get_highest_prio(active_queue);
    if (highest_prio == -1) {
        std::list<Process *>* temp = active_queue;
        active_queue = expired_queue;
        expired_queue = temp;
        highest_prio = get_highest_prio(active_queue);
        // both active queue and expired queue are empty
        if (highest_prio == -1) {
            return NULL;
        }
    }

    Process *process = active_queue[highest_prio].front();
    active_queue[highest_prio].pop_front();


    process->cpu_waiting_time += current_time - process->latest_enqueue_time;
    return process;
}

int PrioScheduler::get_highest_prio(std::list<Process *> *queues)
{
    for (int i = maxprio - 1; i >=0; i--) {
        if (!queues[i].empty()) {
            return i;
        }
    }
    return -1;
}

bool PrioScheduler::test_preempt(Process *process, int quantum)
{
    if (process->cpu_burst > quantum)
    {
        return true;
    }
    return false;
}

// PrePrior

PrePrioScheduler::PrePrioScheduler(int maxprio)
{
    this->maxprio = maxprio;
    active_queue = new std::list<Process *>[maxprio];
    expired_queue = new std::list<Process *>[maxprio];
}

void PrePrioScheduler::add_process(Process *process, int current_time)
{
    process->latest_enqueue_time = current_time;

    int priority = process->dynamic_priority;
    if (priority == -1)
    {
        process->dynamic_priority = process->static_priority - 1;
        expired_queue[process->dynamic_priority].push_back(process);
    }
    else
    {
        active_queue[process->dynamic_priority].push_back(process);
    }
}

Process *PrePrioScheduler::get_next_process(int current_time)
{
    int highest_prio = get_highest_prio(active_queue);
    if (highest_prio == -1) {
        std::list<Process *>* temp = active_queue;
        active_queue = expired_queue;
        expired_queue = temp;
        highest_prio = get_highest_prio(active_queue);
        // both active queue and expired queue are empty
        if (highest_prio == -1) {
            return NULL;
        }
    }

    Process *process = active_queue[highest_prio].front();
    active_queue[highest_prio].pop_front();


    process->cpu_waiting_time += current_time - process->latest_enqueue_time;
    return process;
}

int PrePrioScheduler::get_highest_prio(std::list<Process *> *queues)
{
    for (int i = maxprio - 1; i >=0; i--) {
        if (!queues[i].empty()) {
            return i;
        }
    }
    return -1;
}

bool PrePrioScheduler::test_preempt(Process *process, int quantum)
{
    if (process->cpu_burst > quantum)
    {
        return true;
    }
    return false;
}

bool PrePrioScheduler::test_prior_preempt(Process * current_running_process, Process * wake_up_process) 
{
    if (current_running_process == NULL)
    {
        return false;
    }

    if (wake_up_process->dynamic_priority > current_running_process->dynamic_priority)
    {
        return true;
    } else {
        return false;
    }
}