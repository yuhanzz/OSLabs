#include <iostream>
#include <list>
#include "Process.h"

extern bool verbose;

typedef enum
{
    TRANS_TO_READY,
    TRANS_TO_RUN,
    TRANS_TO_BLOCK,
    TRANS_TO_PREEMPT
} TransitionType;

class Event
{
public:
    int time_stamp;
    Process *process;
    ProcessState old_state;
    ProcessState new_state;
    TransitionType state_transition;
    Event(int, Process *, TransitionType, ProcessState, ProcessState);
    // friend bool operator< (const Event &, const Event &);
};

class EventQueue
{
public:
    std::list<Event *> queue;
    void add_event(Event *);
    bool get_event(Event *&);
    int get_next_event_time();
    bool exist_event(int, int);
    void rm_future_events(int);

    // for debug
    void printQueue();
};