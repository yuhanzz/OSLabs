#include <iostream>
#include "Process.h"

int Process::available_pid = 0;

Process::Process(int AT, int TC, int CB, int IO, int static_priority) {
    pid = available_pid++;
    arrive_time = AT;
    total_cpu = TC;
    cpu_burst_max = CB;
    io_burst_max = IO;
    latest_trans_time = AT;

    remaining_cpu = total_cpu;
    cpu_waiting_time = 0;

    this->static_priority = static_priority;
    dynamic_priority = static_priority - 1;
    total_io_time = 0;
    cpu_burst = 0;
    io_burst = 0;
}