#include <iostream>
#include <list>
#include "Process.h"



class BaseScheduler
{
public:
    std::list<Process*> active_queue;
    virtual void add_process(Process *, int) = 0;
    virtual Process *get_next_process(int);
    virtual bool test_preempt(Process *, int) {return false; };
};

class FcfsScheduler : public BaseScheduler
{
public:
    void add_process(Process *, int);
};

class LcfsScheduler : public BaseScheduler
{
public:
    void add_process(Process *, int);
};

class SrtfScheduler : public BaseScheduler
{
public:
    void add_process(Process *, int);
};

class RrScheduler : public BaseScheduler
{
public:
    void add_process(Process *, int);
    bool test_preempt(Process *, int);
};