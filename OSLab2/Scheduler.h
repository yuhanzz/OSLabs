#include <iostream>
#include <list>
#include "Process.h"



class BaseScheduler
{
public:
    std::list<Process*> active_queue;
    virtual void add_process(Process *, int) = 0;
    virtual Process *get_next_process(int) = 0;
    virtual void test_preempt(Process *, int) {};
};

class FcfsScheduler : public BaseScheduler
{
public:
    void add_process(Process *, int);
    Process *get_next_process(int);
};

class LcfsScheduler : public BaseScheduler
{
public:
    void add_process(Process *, int);
    Process *get_next_process(int);
};