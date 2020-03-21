#include <iostream>
#include <list>
#include "Process.h"

class BaseScheduler
{
public:
    virtual void add_process(Process *, int) = 0;
    virtual Process *get_next_process(int) = 0;
    virtual bool test_preempt(Process *, int) { return false; };
};

class FcfsScheduler : public BaseScheduler
{
public:
    std::list<Process *> active_queue;
    void add_process(Process *, int);
    Process *get_next_process(int);
};

class LcfsScheduler : public BaseScheduler
{
public:
    std::list<Process *> active_queue;
    void add_process(Process *, int);
    Process *get_next_process(int);
};

class SrtfScheduler : public BaseScheduler
{
public:
    std::list<Process *> active_queue;
    void add_process(Process *, int);
    Process *get_next_process(int);
};

class RrScheduler : public BaseScheduler
{
public:
    std::list<Process *> active_queue;
    void add_process(Process *, int);
    Process *get_next_process(int);
    bool test_preempt(Process *, int);
};

class PrioScheduler : public BaseScheduler
{
public:
    int maxprio;
    std::list<Process *>* active_queue;
    std::list<Process *>* expired_queue;
    void add_process(Process *, int);
    Process *get_next_process(int);
    bool test_preempt(Process *, int);

    PrioScheduler(int maxprio);
    int get_highest_prio(std::list<Process *>*);
};