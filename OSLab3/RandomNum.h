#include <iostream>
#include <fstream>

class RandomNum
{
public:
    int *randvals;
    int ofs;
    int totalCount;
    RandomNum(std::string file_name);
    int get(int);
};