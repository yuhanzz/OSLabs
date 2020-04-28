#include <iostream>
#include <climits>
#include <cstdint>
#include "Process.h"

class Pager
{
public:
    virtual int select_victim(int) = 0;
};

class FifoPager : public Pager
{
public:
    int frame_count;
    int hand;
    int select_victim(int);
    FifoPager(int frame_count)
    {
        this->hand = 0;
        this->frame_count = frame_count;
    }
};

class ClockPager : public Pager
{
public:
    int frame_count;
    Process *process_table;
    std::pair<int, int> *frame_table;

    int hand;
    int select_victim(int);
    ClockPager(int frame_count, Process *process_table, std::pair<int, int> *frame_table)
    {
        this->hand = 0;
        this->frame_count = frame_count;
        this->process_table = process_table;
        this->frame_table = frame_table;
    }
};

class NruPager : public Pager
{
public:
    int frame_count;
    Process *process_table;
    std::pair<int, int> *frame_table;

    int hand;
    int last_reset_instr_count;
    int select_victim(int);
    NruPager(int frame_count, Process *process_table, std::pair<int, int> *frame_table)
    {
        this->hand = 0;
        this->frame_count = frame_count;
        this->process_table = process_table;
        this->frame_table = frame_table;
        this->last_reset_instr_count = 0;
    }
};


class WorkingSetPager : public Pager
{
public:
    int frame_count;
    Process *process_table;
    std::pair<int, int> *frame_table;
    int* time_last_used_table;

    int hand;
    int select_victim(int);
    WorkingSetPager(int frame_count, Process *process_table, std::pair<int, int> *frame_table, int* time_last_used_table)
    {
        this->hand = 0;
        this->frame_count = frame_count;
        this->process_table = process_table;
        this->frame_table = frame_table;
        this->time_last_used_table = time_last_used_table;
    }
};

class AgingPager : public Pager
{
public:
    int frame_count;
    Process *process_table;
    std::pair<int, int> *frame_table;
    uint32_t* age_table;

    int hand;
    int select_victim(int);
    AgingPager(int frame_count, Process *process_table, std::pair<int, int> *frame_table, uint32_t* age_table)
    {
        this->hand = 0;
        this->frame_count = frame_count;
        this->process_table = process_table;
        this->frame_table = frame_table;
        this->age_table = age_table;
    }
};