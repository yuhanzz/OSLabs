#include<iostream>
#include "RandomNum.h"

RandomNum::RandomNum(std::string file_name) {
    // ?? = 0 ??
    ofs = 0;

    std::ifstream infile;
    infile.open(file_name, std::ios::in);

    std::string line;
    std::getline(infile, line);
    totalCount = std::stoi(line);

    randvals = new int[totalCount];
    for (int i = 0; i < totalCount; i++) {
        std::getline(infile, line);
        randvals[i] = std::stoi(line);
    }
}

int RandomNum::get(int burst) {
    int num = 1 + (randvals[ofs] % burst);
    ofs++;

    if (ofs == totalCount) {
        ofs = 0;
    }
    return num;
}