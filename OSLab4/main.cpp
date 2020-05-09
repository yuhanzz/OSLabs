#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <cmath>
#include <list>
#include "Scheduler.h"

std::ifstream infile;
std::list<IORequest> request_queue;
std::list<IORequest> io_queue;
IORequest *current_request;

int current_time = 0;
int head = 0;

std::string get_next_line()
{
    std::string line;
    while (!infile.eof())
    {
        std::getline(infile, line);
        if (line[line.find_first_not_of(" \t")] != '#')
        {
            return line;
        }
    }
    return "";
}

bool get_next_instruction(int &time_step, int &track)
{
    std::string line = get_next_line();
    if (line.length() != 0)
    {
        std::stringstream ss(line);
        ss >> time_step >> track;
        return true;
    }
    else
    {
        return false;
    }
}

void print_summary()
{
    int request_count = request_queue.size();
    int total_movement = 0;
    int total_turnaround = 0;
    int total_waittime = 0;
    int max_waittime = 0;
    for (std::list<IORequest>::iterator iter = request_queue.begin(); iter != request_queue.end(); iter++)
    {
        total_movement += abs(iter->end_track - iter->start_track);
        total_turnaround += iter->end_time - iter->arrive_time;

        int wait_time = iter->start_time - iter->arrive_time;
        total_waittime += wait_time;
        max_waittime = wait_time > max_waittime ? wait_time : max_waittime;
        printf("%5d: %5d %5d %5d\n", iter->op_num, iter->arrive_time, iter->start_time, iter->end_time);
    }

    double avg_turnaround = total_turnaround / (double)request_count;
    double avg_waittime = total_waittime / (double)request_count;

    printf("SUM: %d %d %.2lf %.2lf %d\n", current_time - 1, total_movement, avg_turnaround, avg_waittime, max_waittime);
}

int main(int argc, char **argv)
{
    FifoScheduler scheduler;

    infile.open(argv[1], std::ios::in);

    int time_step, track;
    int op_num = 0;
    while (get_next_instruction(time_step, track))
    {
        request_queue.push_back(IORequest(op_num, time_step, track));
        op_num++;
    }

    std::list<IORequest>::iterator request_iter = request_queue.begin();
    while (request_iter != request_queue.end() || current_request != NULL || !scheduler.io_queue.empty())
    {
        if (request_iter->arrive_time == current_time)
        {
            scheduler.enqueue(&(*request_iter));
            request_iter++;
        }

        if (current_request != NULL && head == current_request->end_track)
        {
            current_request->end_time = current_time;
            current_request = NULL;
        }

        if (current_request != NULL && head != current_request->end_track)
        {
            int step = current_request->end_track - head > 0 ? 1 : -1;
            head += step;
        }

        if (current_request == NULL)
        {
            if (!scheduler.io_queue.empty())
            {
                current_request = scheduler.strategy();
                current_request->start_time = current_time;
                current_request->start_track = head;

                int step = current_request->end_track - head > 0 ? 1 : -1;
                head += step;
            }
        }
        // when scheduled, remember to set head to the current track;

        current_time++;
    }

    print_summary();

    // while (!request_queue.empty())
    // {
    //     IORequest request = request_queue.front();
    //     request_queue.pop_front();
    //     std::cout << request.arrive_time << " " << request.track << std::endl;
    // }
}