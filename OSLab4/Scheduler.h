#include <iostream>
#include "IORequest.h"
#include <list>
#include <climits>
#include <cmath>

class BaseScheduler
{
public:
    std::list<IORequest *> io_queue;
    virtual void enqueue(IORequest *) = 0;
};

class FifoScheduler : public BaseScheduler
{
public:
    IORequest *strategy(int);
    void enqueue(IORequest *);
};

class SstfScheduler : public BaseScheduler
{
    public:
    IORequest *strategy(int);
    void enqueue(IORequest *);
};