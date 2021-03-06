#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <unistd.h>
#include "Process.h"
#include "EventQueue.h"
#include "Scheduler.h"
#include "RandomNum.h"

RandomNum *random_generator;
EventQueue event_queue;
Process *current_running_process;
std::list<Process *> process_list;
bool verbose = false;
bool t_verbose = false;
bool e_verbose = false;
int quantum;
int maxprio = 4;

int last_finishing_time;
int all_process_total_cpu;
int latest_io_time;
int io_idle_time;

std::string ProcessStateStrings[] = {"CREATED", "READY", "RUNNG", "BLOCK"};

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

void simulation(char sched_type)
{
    BaseScheduler *scheduler;
    switch (sched_type)
    {
    case 'F':
        std::cout << "FCFS" << std::endl;
        scheduler = new FcfsScheduler();
        break;
    case 'L':
        std::cout << "LCFS" << std::endl;
        scheduler = new LcfsScheduler();
        break;
    case 'S':
        std::cout << "SRTF" << std::endl;
        scheduler = new SrtfScheduler();
        break;
    case 'R':
        std::cout << "RR " << quantum << std::endl;
        scheduler = new RrScheduler();
        break;
    case 'P':
        std::cout << "PRIO " << quantum << std::endl;
        scheduler = new PrioScheduler(maxprio);
        break;
    case 'E':
        std::cout << "PREPRIO " << quantum << std::endl;
        scheduler = new PrePrioScheduler(maxprio);
        break;
    }

    Event *event;
    bool call_scheduler = false;
    while (event_queue.get_event(event))
    {
        Process *process = event->process;
        int current_time = event->time_stamp;
        int prev_state_time = current_time - process->latest_trans_time;
        process->latest_trans_time = current_time;

        switch (event->state_transition)
        {
        case TRANS_TO_READY:
        {
            if (event->old_state == BLOCKED)
            {
                process->dynamic_priority = process->static_priority - 1;
            }
            scheduler->add_process(process, current_time);
            if (current_running_process == NULL)
            {
                call_scheduler = true;
            }
            else
            {
                if (scheduler->test_prior_preempt(current_running_process, process))
                {
                    // check if there is pending event at the same time, if not, then preempt
                    if (!event_queue.exist_event(current_running_process->pid, current_time))
                    {
                        Event *prio_preempt_event = new Event(current_time, current_running_process, TRANS_TO_PREEMPT, RUNNING, READY);
                        event_queue.add_event(prio_preempt_event);
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

            if (scheduler->test_preempt(process, quantum))
            {
                Event *toPreemptedEvent_R = new Event(current_time + quantum, process, TRANS_TO_PREEMPT, RUNNING, READY);
                event_queue.add_event(toPreemptedEvent_R);
            }
            else
            {
                Event *toBlockedEvent_R = new Event(current_time + process->cpu_burst, process, TRANS_TO_BLOCK, RUNNING, BLOCKED);
                event_queue.add_event(toBlockedEvent_R);
            }

            call_scheduler = false;
        }
        break;
        case TRANS_TO_BLOCK:
        {

            current_running_process = NULL;

            process->remaining_cpu -= process->cpu_burst;
            process->cpu_burst = 0;

            if (process->remaining_cpu == 0)
            {
                process->finishing_time = current_time;
                if (event_queue.queue.empty())
                {
                    last_finishing_time = current_time;
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
            int cpu_burst_compeleted = prev_state_time;
            process->cpu_burst -= cpu_burst_compeleted;
            process->remaining_cpu -= cpu_burst_compeleted;

            process->dynamic_priority--;

            // rm future events
            event_queue.rm_future_events(process->pid);

            scheduler->add_process(process, current_time);

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
                Process *scheduled_process = scheduler->get_next_process(current_time);
                // if there is no process in active queue
                if (scheduled_process == NULL)
                {
                    continue;
                }
                
                Event *toRunEvent = new Event(current_time, scheduled_process, TRANS_TO_RUN, READY, RUNNING);
                event_queue.add_event(toRunEvent);
            }
        }
    }
    io_idle_time += last_finishing_time - latest_io_time;
}

void print_result()
{
    for (std::list<Process *>::iterator iterator = process_list.begin(); iterator != process_list.end(); iterator++)
    {
        printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n", (*iterator)->pid, (*iterator)->arrive_time, (*iterator)->total_cpu, (*iterator)->cpu_burst_max, (*iterator)->io_burst_max, (*iterator)->static_priority, (*iterator)->finishing_time, (*iterator)->finishing_time - (*iterator)->arrive_time, (*iterator)->total_io_time, (*iterator)->cpu_waiting_time);
    }
    double cpu_utilization = (all_process_total_cpu / (double)last_finishing_time) * 100;
    double io_utilization = ((last_finishing_time - io_idle_time) / (double)last_finishing_time) * 100;

    int process_count = 0;
    int total_turn_around = 0;
    int total_cpu_waiting = 0;
    for (std::list<Process *>::iterator iterator = process_list.begin(); iterator != process_list.end(); iterator++)
    {
        process_count++;
        total_turn_around += (*iterator)->finishing_time - (*iterator)->arrive_time;
        total_cpu_waiting += (*iterator)->cpu_waiting_time;
    }
    double ave_turn_around = total_turn_around / (double)process_count;
    double ave_cpu_waiting = total_cpu_waiting / (double)process_count;
    double throughput = (process_count / (double)(last_finishing_time)) * 100;

    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n", last_finishing_time, cpu_utilization, io_utilization, ave_turn_around, ave_cpu_waiting, throughput);
}

int main(int argc, char **argv)
{

    // get verbose options, s_value
    int c;
    char *s_value = NULL;

    while ((c = getopt(argc, argv, "vtes:")) != -1)
        switch (c)
        {
        case 'v':
            verbose = true;
            break;
        case 't':
            t_verbose = true;
            break;
        case 'e':
            e_verbose = true;
            break;
        case 's':
            s_value = optarg;
            break;
        }

    char op1;
    int op2, op3;
    // parse s_value, confirm quantum, maxprio
    if (s_value[0] == 'R' || s_value[0] == 'P' || s_value[0] == 'E')
    {
        int opCount = sscanf(s_value, "%c%d:%d", &op1, &op2, &op3);
        if (opCount == 3)
        {
            quantum = op2;
            maxprio = op3;
        }
        else
        {
            quantum = op2;
        }
    }

    // initialize global variable
    all_process_total_cpu = 0;
    latest_io_time = 0;
    io_idle_time = 0;
    random_generator = new RandomNum(argv[optind + 1]);

    // read from file and add initial event for each line
    std::ifstream infile;
    infile.open(argv[optind], std::ios::in);
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

    // start simulation
     simulation(s_value[0]);

    // print info
    print_result();
}