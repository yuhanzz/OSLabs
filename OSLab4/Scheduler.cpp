#include "Scheduler.h"

void FifoScheduler::enqueue(IORequest *request)
{
    io_queue.push_back(request);
}

IORequest *FifoScheduler::strategy(int head)
{
    IORequest* selected_request = io_queue.front();
    io_queue.pop_front();
    return selected_request;
}

void SstfScheduler::enqueue(IORequest *request)
{
    io_queue.push_back(request);
}

IORequest *SstfScheduler::strategy(int head)
{
    IORequest* st_request;
    int shortest_time = INT_MAX;
    for(std::list<IORequest*>::iterator iter = io_queue.begin(); iter != io_queue.end(); iter++)
    {
        int seek_time = abs((*iter)->end_track - head);
        if (seek_time < shortest_time)
        {
            st_request = *iter;
            shortest_time = seek_time;
        }
    }
    io_queue.remove(st_request);
    return st_request;
}