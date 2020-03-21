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
int quantum = 2;

int last_finishing_time;
int all_process_total_cpu;
int latest_io_time;
int io_idle_time;

std::string ProcessStateStrings[] = {"CREATED", "READY", "RUNNG", "BLOCK"};

// TODO
int maxprio = 5;

void print_verbose(Event *event, int current_time, int prev_state_time)
{
    std::cout << current_time << " " << event->process->pid << " " << prev_state_time << ": ";
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
        std::cout << " cb=" << event->process->cpu_burst << " rem=" << rem << " prio=" << event->process->dynamic_priority;
    }
    if (event->state_transition == TRANS_TO_PREEMPT)
    {
        std::cout << "  cb=" << event->process->cpu_burst << " rem=" << rem << " prio=" << event->process->dynamic_priority;
    }
    std::cout << std::endl;
}

void simulation()
{
    PrePrioScheduler scheduler(maxprio);
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
            if (event->old_state == BLOCKED)
            {
                process->dynamic_priority = process->static_priority - 1;
            }
            scheduler.add_process(process, current_time);
            if (current_running_process == NULL)
            {
                call_scheduler = true;
            } else {
                if (scheduler.test_prior_preempt(current_running_process, process))
                {
                    // check if there is pending event at the same time, if not, then preempt
                    if (!event_queue.exist_event(current_running_process->pid, current_time))
                    {
                        std::cout << "yes" << std::endl;
                        Event* prio_preempt_event = new Event(current_time, current_running_process, TRANS_TO_PREEMPT, RUNNING, READY);
                        event_queue.add_event(prio_preempt_event);
                    } else {
                        std::cout << "no" << std::endl;
                    }
                }
            }
        }
        break;
        case TRANS_TO_RUN:
        {
            current_running_process = process;

            if (process->cpu_burst == 0)
                process->cpu_burst = std::min(random_generator->get(process->cpu_burst_max), process->remaining_cpu);

            if (scheduler.test_preempt(process, quantum))
            {
                Event *toPreemptedEvent_R = new Event(current_time + quantum, process, TRANS_TO_PREEMPT, RUNNING, READY);
                event_queue.add_event(toPreemptedEvent_R);
            }
            else
            {
                Event *toBlockedEvent_R = new Event(current_time + process->cpu_burst, process, TRANS_TO_BLOCK, RUNNING, BLOCKED);
                event_queue.add_event(toBlockedEvent_R);
            }

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

            if (process->remaining_cpu == 0)
            {
                process->finishing_time = current_time;
                if (event_queue.queue.empty())
                {
                    last_finishing_time = current_time;
                    io_idle_time += current_time - latest_io_time;
                }
            }
            else
            {

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
            // 目前来说process和current_running_process肯定是同一个
            // 千万要直接用prev_state_time，因为trans_time已经在开头被改掉了
            int cpu_burst_compeleted = prev_state_time;
            process->cpu_burst -= cpu_burst_compeleted;
            process->remaining_cpu -= cpu_burst_compeleted;

            process->dynamic_priority --;

            // rm future events
            event_queue.rm_future_events(process->pid);

            scheduler.add_process(process, current_time);

            current_running_process = NULL;
            call_scheduler = true;
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