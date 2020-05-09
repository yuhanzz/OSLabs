#include <iostream>
#include "IORequest.h"
#include <list>
#include <climits>
#include <cmath>

class BaseScheduler
{
public:
    std::list<IORequest *> *io_queue;
    virtual void enqueue(IORequest *) = 0;
    virtual bool is_empty() = 0;
};

class FifoScheduler : public BaseScheduler
{
public:
    IORequest *strategy(int);
    void enqueue(IORequest *);
    FifoScheduler()
    {
        io_queue = new std::list<IORequest *>();
    }
    bool is_empty()
    {
        return io_queue->empty();
    }
};

class SstfScheduler : public BaseScheduler
{
public:
    IORequest *strategy(int);
    void enqueue(IORequest *);
    SstfScheduler()
    {
        io_queue = new std::list<IORequest *>();
    }
    bool is_empty()
    {
        return io_queue->empty();
    }
};

class LookScheduler : public BaseScheduler
{
public:
    int direction;
    IORequest *strategy(int);
    void enqueue(IORequest *);
    LookScheduler()
    {
        io_queue = new std::list<IORequest *>();
        direction = 1;
    }
    bool is_empty()
    {
        return io_queue->empty();
    }
};

class CLookScheduler : public BaseScheduler
{
public:
    IORequest *strategy(int);
    void enqueue(IORequest *);
    CLookScheduler()
    {
        io_queue = new std::list<IORequest *>();
    }
    bool is_empty()
    {
        return io_queue->empty();
    }
};

class FLookScheduler : public BaseScheduler
{
public:
    int direction;
    std::list<IORequest *> *add_queue;
    IORequest *strategy(int);
    IORequest *look(int);
    void enqueue(IORequest *);
    FLookScheduler()
    {
        io_queue = new std::list<IORequest *>();
        add_queue = new std::list<IORequest *>();
        direction = 1;
    }
    bool is_empty()
    {
        return io_queue->empty() && add_queue->empty();
    }
};