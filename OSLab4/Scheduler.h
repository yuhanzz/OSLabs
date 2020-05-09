#include <iostream>
#include "IORequest.h"
#include <list>

class BaseScheduler
{
public:
    std::list<IORequest *> io_queue;
    virtual IORequest *strategy() = 0;
    virtual void enqueue(IORequest *) = 0;
};

class FifoScheduler : public BaseScheduler
{
public:
    IORequest *strategy();
    void enqueue(IORequest *);
};