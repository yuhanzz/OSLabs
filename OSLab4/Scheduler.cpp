#include "Scheduler.h"

void FifoScheduler::enqueue(IORequest *request)
{
    io_queue.push_back(request);
}

IORequest *FifoScheduler::strategy()
{
    IORequest* selected_request = io_queue.front();
    io_queue.pop_front();
    return selected_request;
}