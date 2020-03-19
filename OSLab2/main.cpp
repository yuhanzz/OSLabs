#include <iostream>
#include <fstream>
#include <cstring>
#include "EventQueue.h"

int main(int argc, char *argv[]) {
    EventQueue eventQueue;

    // read from file
    std::ifstream infile;
    infile.open(argv[1], std::ios::in);
    std::string line;
    char *element = NULL;
    char *charArr;
    while(!infile.eof()) {
        std::getline(infile, line);
        charArr = new char[line.length() + 1];
        strcpy(charArr, line.c_str());

        element = strtok(charArr, " \t");
        if (element == NULL) {
            break;
        }
        int AT = std::stoi(std::string(element));

        element = strtok(NULL, " \t");
        int TC = std::stoi(std::string(element));

        element = strtok(NULL, " \t");
        int CB = std::stoi(std::string(element));

        element = strtok(NULL, " \t");
        int IO = std::stoi(std::string(element));

        std::cout << AT << '\t' << TC << '\t' << CB << '\t' << IO << std::endl;
    }
}