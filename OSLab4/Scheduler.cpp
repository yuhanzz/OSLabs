#include "Scheduler.h"

void FifoScheduler::enqueue(IORequest *request)
{
    io_queue->push_back(request);
}

IORequest *FifoScheduler::strategy(int head)
{
    IORequest *selected_request = io_queue->front();
    io_queue->pop_front();
    return selected_request;
}

void SstfScheduler::enqueue(IORequest *request)
{
    io_queue->push_back(request);
}

IORequest *SstfScheduler::strategy(int head)
{
    IORequest *st_request;
    int shortest_time = INT_MAX;
    for (std::list<IORequest *>::iterator iter = io_queue->begin(); iter != io_queue->end(); iter++)
    {
        int seek_time = abs((*iter)->end_track - head);
        if (seek_time < shortest_time)
        {
            st_request = *iter;
            shortest_time = seek_time;
        }
    }
    io_queue->remove(st_request);
    return st_request;
}

void LookScheduler::enqueue(IORequest *request)
{
    io_queue->push_back(request);
}

IORequest *LookScheduler::strategy(int head)
{
    IORequest *selected_request;
    if (direction == 1)
    {
        int closest_track = INT_MAX;
        int largest_track = -1;
        IORequest *largest_track_request;
        for (std::list<IORequest *>::iterator iter = io_queue->begin(); iter != io_queue->end(); iter++)
        {
            if ((*iter)->end_track >= head && (*iter)->end_track < closest_track)
            {
                closest_track = (*iter)->end_track;
                selected_request = (*iter);
            }
            if ((*iter)->end_track > largest_track)
            {
                largest_track = (*iter)->end_track;
                largest_track_request = (*iter);
            }
        }
        if (closest_track == INT_MAX)
        {
            direction = -1;
            selected_request = largest_track_request;
        }
    }
    else
    {
        int closest_track = -1;
        int smallest_track = INT_MAX;
        IORequest *smallest_track_request;
        for (std::list<IORequest *>::iterator iter = io_queue->begin(); iter != io_queue->end(); iter++)
        {
            if ((*iter)->end_track <= head && (*iter)->end_track > closest_track)
            {
                closest_track = (*iter)->end_track;
                selected_request = (*iter);
            }
            if ((*iter)->end_track < smallest_track)
            {
                smallest_track = (*iter)->end_track;
                smallest_track_request = (*iter);
            }
        }
        if (closest_track == -1)
        {
            direction = 1;
            selected_request = smallest_track_request;
        }
    }

    io_queue->remove(selected_request);
    return selected_request;
}

void CLookScheduler::enqueue(IORequest *request)
{
    io_queue->push_back(request);
}

IORequest *CLookScheduler::strategy(int head)
{
    IORequest *selected_request;
    int closest_track = INT_MAX;
    int smallest_track = INT_MAX;
    IORequest *smallest_track_request;
    for (std::list<IORequest *>::iterator iter = io_queue->begin(); iter != io_queue->end(); iter++)
    {
        if ((*iter)->end_track >= head && (*iter)->end_track < closest_track)
        {
            closest_track = (*iter)->end_track;
            selected_request = (*iter);
        }
        if ((*iter)->end_track < smallest_track)
        {
            smallest_track = (*iter)->end_track;
            smallest_track_request = (*iter);
        }
    }
    if (closest_track == INT_MAX)
    {
        selected_request = smallest_track_request;
    }

    io_queue->remove(selected_request);
    return selected_request;
}

void FLookScheduler::enqueue(IORequest *request)
{
    add_queue->push_back(request);
}

IORequest *FLookScheduler::strategy(int head)
{
    if (io_queue->empty())
    {
        io_queue = add_queue;
        add_queue = new std::list<IORequest *>();
    }
    return look(head);
}

IORequest *FLookScheduler::look(int head)
{
    IORequest *selected_request;
    if (direction == 1)
    {
        int closest_track = INT_MAX;
        int largest_track = -1;
        IORequest *largest_track_request;
        for (std::list<IORequest *>::iterator iter = io_queue->begin(); iter != io_queue->end(); iter++)
        {
            if ((*iter)->end_track >= head && (*iter)->end_track < closest_track)
            {
                closest_track = (*iter)->end_track;
                selected_request = (*iter);
            }
            if ((*iter)->end_track > largest_track)
            {
                largest_track = (*iter)->end_track;
                largest_track_request = (*iter);
            }
        }
        if (closest_track == INT_MAX)
        {
            direction = -1;
            selected_request = largest_track_request;
        }
    }
    else
    {
        int closest_track = -1;
        int smallest_track = INT_MAX;
        IORequest *smallest_track_request;
        for (std::list<IORequest *>::iterator iter = io_queue->begin(); iter != io_queue->end(); iter++)
        {
            if ((*iter)->end_track <= head && (*iter)->end_track > closest_track)
            {
                closest_track = (*iter)->end_track;
                selected_request = (*iter);
            }
            if ((*iter)->end_track < smallest_track)
            {
                smallest_track = (*iter)->end_track;
                smallest_track_request = (*iter);
            }
        }
        if (closest_track == -1)
        {
            direction = 1;
            selected_request = smallest_track_request;
        }
    }

    io_queue->remove(selected_request);
    return selected_request;
}
