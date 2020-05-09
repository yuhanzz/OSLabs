#include <iostream>

class IORequest
{
public:
    int op_num;
    int arrive_time;
    int end_track;
    int start_time;
    int end_time;

    int start_track;



    IORequest(int op_num, int arrive_time, int end_track)
    {
        this->op_num = op_num;
        this->arrive_time = arrive_time;
        this->end_track = end_track;
    }
};