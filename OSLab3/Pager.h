#include <iostream>

class Pager
{
public:
    virtual int select_victim() = 0;
};

class FifoPager : public Pager
{
public:
    int frame_count;
    int hand;
    int select_victim();
    FifoPager(int frame_count)
    {
        this->hand = 0;
        this->frame_count = frame_count;
    }
};