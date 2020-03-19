#include <iostream>
#include <list>

class Event
{
    int timeStamp;
    int process;
    int oldState; // change to enum
    int newState; // change to enum
    Event(int timeStamp, int process, int oldState, int newState)
    {
        this->timeStamp = timeStamp;
        this->process = process;
        this->oldState = oldState;
        this->newState = newState;
    }
};

class EventQueue
{
    std::list<Event *> queue;
};