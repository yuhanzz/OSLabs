#include <iostream>
#include "EventQueue.h"


Event::Event(int time_stamp, Process *process, TransitionType state_transition, ProcessState old_state, ProcessState new_state)
{
    this->time_stamp = time_stamp;
    this->process = process;
    this->state_transition = state_transition;
    this->old_state = old_state;
    this->new_state = new_state;
}

void EventQueue::add_event(Event *event)
{
    int current_time = event->time_stamp;

    // a linear seach to find a place where the timeStamp is between two events

    if (queue.empty())
    {
        queue.push_front(event);
        return;
    }

    std::list<Event *>::iterator iterator;

    for (iterator = queue.begin(); iterator != queue.end(); iterator++)
    {
        int compared_time = (*iterator)->time_stamp;
        if (compared_time > current_time)
        {
            queue.insert(iterator, event);
            return;
        }
    }
    queue.push_back(event);
}

bool EventQueue::get_event(Event *&event)
{
    if (queue.empty())
    {
        return false;
    }
    event = queue.front();
    queue.pop_front();

    return true;
}

void EventQueue::printQueue()
{
    for (std::list<Event *>::iterator iterator = queue.begin(); iterator != queue.end(); ++iterator)
    {
        std::cout << (*iterator)->process->pid << "\t" << (*iterator)->process->arrive_time << "\t" << (*iterator)->process->total_cpu << "\t" << (*iterator)->process->cpu_burst_max << "\t" << (*iterator)->process->io_burst_max << "\t" << std::endl;
        std::cout << (*iterator)->time_stamp << "\t" << (*iterator)->state_transition << std::endl;
    }
}

int EventQueue::get_next_event_time()
{
    if (queue.empty())
    {
        // TODO
        // should this be -1 ?
        return -1;
    }
    return queue.front()->time_stamp;
}