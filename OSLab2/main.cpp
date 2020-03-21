#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include "Process.h"
#include "EventQueue.h"
#include "Scheduler.h"
#include "RandomNum.h"

RandomNum *random_generator;
EventQueue event_queue;
Process *current_running_process;
std::list<Process *> process_list;
bool verbose = true;

int last_finishing_time;
int all_process_total_cpu;
int latest_io_time;
int io_idle_time;

std::string ProcessStateStrings[] = {"CREATED", "READY", "RUNNING", "BLOCKED"};

// TODO
int maxprio = 4;

void print_verbose(Event *event, int current_time, int prev_state_time)
{
    std::cout << current_time << " " << event->process->pid << " " << prev_state_time << " ";
    int rem = event->process->remaining_cpu;
    if (rem != 0)
    {
        std::cout << ProcessStateStrings[event->old_state] << " -> " << ProcessStateStrings[event->new_state];
    }
    else
    {
        std::cout << "Done" << std::endl;
        return;
    }
    if (event->state_transition == TRANS_TO_BLOCK)
    {
        std::cout << " ib=" << event->process->io_burst << " rem=" << rem;
    }
    if (event->state_transition == TRANS_TO_RUN)
    {
        std::cout << " cb=" << event->process->cpu_burst << " rem=" << rem << " prior=" << event->process->dynamic_priority;
    }
    std::cout << std::endl;
}

void simulation()
{
    FcfsScheduler scheduler;
    Event *event;
    bool call_scheduler = false;
    while (event_queue.get_event(event))
    {
        Process *process = event->process;
        int current_time = event->time_stamp;
        int prev_state_time = current_time - process->latest_trans_time;
        process->latest_trans_time = current_time;
        // TODO
        // timeInPrevState = CURRENT_TIME – proc->state_ts;

        switch (event->state_transition)
        {
        case TRANS_TO_READY:
        {
            scheduler.add_process(process, current_time);
            if (current_running_process == NULL)
            {
                call_scheduler = true;
            }
        }
        break;
        case TRANS_TO_RUN:
        {
            current_running_process = process;
            // TODO
            // control cpu_burst to be zero to not according to preemption
            if (process->cpu_burst == 0)
                process->cpu_burst = std::min(random_generator->get(process->cpu_burst_max), process->remaining_cpu);
            // TODO: 注意有可能要没有跑完的情况！！！！所以放到哪里减去比较合适呢？？？
            Event *toBlockedEvent = new Event(current_time + process->cpu_burst, process, TRANS_TO_BLOCK, RUNNING, BLOCKED);
            event_queue.add_event(toBlockedEvent);
            // ????
            call_scheduler = false;
            // also change priotiy here!
        }
        break;
        case TRANS_TO_BLOCK:
        {
            // TODO
            // this is only for no preemption situation, assume that every run turns into blocked
            current_running_process = NULL;
            // 注意这两个操作是一体的
            process->remaining_cpu -= process->cpu_burst;
            process->cpu_burst = 0;

            // for debug
            if (process->remaining_cpu < 0) {
                std::cout << process->remaining_cpu;
                return;
            }
            if (process->remaining_cpu == 0)
            {
                // 调用process.termination进行善后工作
                process->finishing_time = current_time;
                if (event_queue.queue.empty())
                {
                    last_finishing_time = current_time;
                    io_idle_time += current_time - latest_io_time;
                }
            }
            else
            {
                // TODO
                // generate an IO burst
                process->io_burst = random_generator->get(process->io_burst_max);
                process->total_io_time += process->io_burst;
                Event *toReadyEvent = new Event(current_time + process->io_burst, process, TRANS_TO_READY, BLOCKED, READY);
                event_queue.add_event(toReadyEvent);

                // for calculating io time
                int start = current_time;
                int end = current_time + process->io_burst;

                // overlap
                if (start <= latest_io_time)
                {
                    latest_io_time = std::max(end, latest_io_time);
                }
                // not overlap
                else
                {
                    io_idle_time += start - latest_io_time;
                    latest_io_time = end;
                }
            }
            call_scheduler = true;
        }
        break;
        case TRANS_TO_PREEMPT:
        {
            // should test preemption ??
            // print verbose
        }
        break;
        }

        if (verbose)
        {
            print_verbose(event, current_time, prev_state_time);
        }

        if (call_scheduler)
        {
            // do not call scheduler until all pending events solved
            if (event_queue.get_next_event_time() == current_time)
            {
                continue;
            }
            // no pending events, call scheduler
            call_scheduler = false;
            // if there is no process running
            if (current_running_process == NULL)
            {
                Process *scheduled_process = scheduler.get_next_process(current_time);
                // if there is no process in active queue
                if (scheduled_process == NULL)
                {
                    continue;
                }
                // TODO
                // if there is process in active queue
                // generate an event for the current running process
                // 这是唯一可能会发生在schedule之后的event?其他event都会发生在schedule之前？
                // Event *event_for_preempted = new Event(current_time, process, TRANS_TO_READY, RUNNING, READY);

                // change current_running_process to scheduled_process
                Event *toRunEvent = new Event(current_time, scheduled_process, TRANS_TO_RUN, READY, RUNNING);
                event_queue.add_event(toRunEvent);
            }
        }
    }
}

void print_result()
{
    for (std::list<Process *>::iterator iterator = process_list.begin(); iterator != process_list.end(); iterator++)
    {
        std::cout << std::setfill('0') << std::setw(3) << (*iterator)->pid << "\t";
        std::cout << (*iterator)->arrive_time << "\t";
        std::cout << (*iterator)->total_cpu << "\t";
        std::cout << (*iterator)->cpu_burst_max << "\t";
        std::cout << (*iterator)->io_burst_max << "\t";
        std::cout << (*iterator)->static_priority << "\t|\t";
        std::cout << (*iterator)->finishing_time << "\t";
        std::cout << (*iterator)->finishing_time - (*iterator)->arrive_time << "\t";
        std::cout << (*iterator)->total_io_time << "\t";
        std::cout << (*iterator)->cpu_waiting_time << "\t";
        std::cout << std::endl;
    }
}

int main(int argc, char *argv[])
{
    // initialize global variable
    all_process_total_cpu = 0;
    latest_io_time = 0;
    io_idle_time = 0;
    random_generator = new RandomNum(argv[2]);

    // read from file and add initial event for each line
    std::ifstream infile;
    infile.open(argv[1], std::ios::in);
    std::string line;
    char *element = NULL;
    char *charArr;
    while (!infile.eof())
    {
        // parse line
        std::getline(infile, line);
        charArr = new char[line.length() + 1];
        strcpy(charArr, line.c_str());
        element = strtok(charArr, " \t");
        if (element == NULL)
        {
            break;
        }
        int AT = std::stoi(std::string(element));
        element = strtok(NULL, " \t");
        int TC = std::stoi(std::string(element));
        element = strtok(NULL, " \t");
        int CB = std::stoi(std::string(element));
        element = strtok(NULL, " \t");
        int IO = std::stoi(std::string(element));

        // create process for each line
        Process *process = new Process(AT, TC, CB, IO, random_generator->get(maxprio));
        process_list.push_back(process);

        // add event for process and enter event into queue
        Event *event = new Event(AT, process, TRANS_TO_READY, CREATED, READY);
        event_queue.add_event(event);

        // for summary info
        all_process_total_cpu += TC;
    }
    // event_queue.printQueue();
    // start simulation
    simulation();

    // print info
    print_result();
}