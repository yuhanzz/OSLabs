#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include "Process.h"

std::ifstream infile;
int process_count;
Process *process_table;

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

bool get_next_instruction(char &operation, int &vpage)
{
    std::string line = get_next_line();
    if (line.length() != 0)
    {
        std::stringstream ss(line);
        ss >> operation >> vpage;
        return true;
    }
    else
    {
        return false;
    }
}

int main(int argc, char **argv)
{

    infile.open(argv[1], std::ios::in);

    // read in process count and create process table
    std::string line = get_next_line();
    std::stringstream ss(line);
    ss >> process_count;
    process_table = new Process[process_count];

    for (int i = 0; i < process_count; i++)
    {
        // modify process
        process_table[i].id = i;

        int vma_count;
        std::string vma_count_line = get_next_line();
        std::stringstream vma_count_ss(vma_count_line);
        vma_count_ss >> vma_count;
        for (int j = 0; j < vma_count; j++)
        {
            int starting_virtual_page, ending_virtual_page, write_protected, filemapped;
            std::string proc_line = get_next_line();
            std::stringstream proc_ss(proc_line);
            proc_ss >> starting_virtual_page >> ending_virtual_page >> write_protected >> filemapped;

            // create vma and add to process
            Vma *vma = new Vma(starting_virtual_page, ending_virtual_page, write_protected, filemapped);
            process_table[i].vma_list.push_back(vma);
        }
    }

    for (int i = 0; i < process_count; i++)
    {
        std::cout << process_table[i].id << std::endl;
        std::list<Vma *> vma_list = process_table[i].vma_list;
        std::list<Vma *>::iterator iterator;

        for (iterator = vma_list.begin(); iterator != vma_list.end(); iterator++)
        {
            std::cout << (*iterator)->starting_virtual_page << " " << (*iterator)->ending_virtual_page << " " << (*iterator)->write_protected << " " << (*iterator)->filemapped << std::endl;
        }
    }

    char operation;
    int vpage;
    while (get_next_instruction(operation, vpage))
    {
        std::cout << operation << " " << vpage << std::endl;
    }
}