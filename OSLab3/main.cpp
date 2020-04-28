#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include "Process.h"
#include "Pager.h"

// needs input
std::ifstream infile;
int process_count;
int frame_count = 32;

Process *process_table;
std::pair<int, int> *frame_table;
int *time_last_used_table;
uint32_t *age_table;
std::list<int> free_list;
Pager *pager;

int current_process;
int current_instr = 0;

int context_switch_count = 0;
int program_exit_count = 0;
int access_count = 0;

void print_frame_table()
{
    std::cout << "FT: ";
    for (int i = 0; i < frame_count; i++)
    {
        if (frame_table[i].first == -1)
        {
            std::cout << "*";
        }
        else
        {
            std::cout << frame_table[i].first << ":" << frame_table[i].second;
        }

        std::cout << " ";
    }
    std::cout << std::endl;
}

void print_page_tables()
{
    for (int i = 0; i < process_count; i++)
    {
        std::cout << "PT[" << i << "]: ";
        process_table[i].print_page_table();
    }
}

void print_process_table()
{
    for (int i = 0; i < process_count; i++)
    {
        std::cout << "PROC[" << i << "]:";
        std::cout << " U=" << process_table[i].unmaps;
        std::cout << " M=" << process_table[i].maps;
        std::cout << " I=" << process_table[i].ins;
        std::cout << " O=" << process_table[i].outs;
        std::cout << " FI=" << process_table[i].fins;
        std::cout << " FO=" << process_table[i].fouts;
        std::cout << " Z=" << process_table[i].zeros;
        std::cout << " SV=" << process_table[i].segv;
        std::cout << " SP=" << process_table[i].segprot;
        std::cout << std::endl;
    }
}

void print_summary_info()
{
    int cost = 0;
    for (int i = 0; i < process_count; i++)
    {
        cost += process_table[i].maps * 400;
        cost += process_table[i].unmaps * 400;
        cost += process_table[i].ins * 3000;
        cost += process_table[i].outs * 3000;
        cost += process_table[i].fins * 2500;
        cost += process_table[i].fouts * 2500;
        cost += process_table[i].zeros * 150;
        cost += process_table[i].segv * 240;
        cost += process_table[i].segprot * 300;
    }
    cost += access_count;
    cost += context_switch_count * 121;
    cost += program_exit_count * 175;

    std::cout << "TOTALCOST " << current_instr << " " << context_switch_count << " " << program_exit_count << " " << cost << std::endl;
}

int get_frame()
{
    int frame;
    if (!free_list.empty())
    {
        frame = free_list.front();
        free_list.pop_front();
    }
    else
    {
        frame = pager->select_victim(current_instr);
        // unmap current user
        int victim_process = frame_table[frame].first;
        int victim_page = frame_table[frame].second;
        PageTableEntry *victim_pte = &process_table[victim_process].page_table[victim_page];

        process_table[victim_process].unmaps++;
        std::cout << " UNMAP " << victim_process << ":" << victim_page << std::endl;

        // save frame to disk if necessary
        if (victim_pte->modified == 1)
        {
            if (victim_pte->filemapped == 1)
            {
                process_table[victim_process].fouts++;
                std::cout << " FOUT" << std::endl;
            }
            else
            {
                victim_pte->paged_out = 1;
                process_table[victim_process].outs++;
                std::cout << " OUT" << std::endl;
            }
        }

        victim_pte->present = 0;
        victim_pte->modified = 0;
        victim_pte->referenced = 0;

        frame_table[frame].first = -1;
    }
    return frame;
}

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

    // --------------- needs modification according to ops ------------------------

    infile.open(argv[1], std::ios::in);
    // remember to get frame_count before the following operations
    frame_table = new std::pair<int, int>[frame_count];
    time_last_used_table = new int[frame_count];
    age_table = new uint32_t[frame_count];
    for (int i = 0; i < frame_count; i++)
    {
        age_table[i] = 0;
    }

    // --------------- needs modification according to ops ------------------------

    // intialize frame table and free list
    for (int i = 0; i < frame_count; i++)
    {
        // -1 represent an unused frame
        frame_table[i].first = -1;
        free_list.push_back(i);
    }

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

    // --------------- needs modification according to ops ------------------------
    // pager = new FifoPager(frame_count);
    // pager = new ClockPager(frame_count, process_table, frame_table);
    // pager = new NruPager(frame_count, process_table, frame_table);
    // pager = new WorkingSetPager(frame_count, process_table, frame_table, time_last_used_table);
    pager = new AgingPager(frame_count, process_table, frame_table, age_table);
    // --------------- needs modification according to ops ------------------------

    // start simulation
    char operation;
    int vpage;
    while (get_next_instruction(operation, vpage))
    {
        std::cout << current_instr << ": ==> " << operation << " " << vpage << std::endl;
        current_instr++;

        if (operation == 'c')
        {
            context_switch_count++;
            current_process = vpage;
            continue;
        }
        if (operation == 'e')
        {
            program_exit_count++;
            std::cout << "EXIT current process " << vpage << std::endl;
            for (int i = 0; i < VIRTUAL_PAGE_COUNT; i++)
            {
                // all the swap space are cleaned when process exits
                process_table[current_process].page_table[i].paged_out = 0;

                if (process_table[current_process].page_table[i].present == 1)
                {
                    process_table[current_process].page_table[i].present = 0;

                    std::cout << " UNMAP " << current_process << ":" << i << std::endl;
                    process_table[current_process].unmaps++;
                    if (process_table[current_process].page_table[i].filemapped == 1 && process_table[current_process].page_table[i].modified == 1)
                    {
                        process_table[current_process].fouts++;
                        std::cout << " FOUT" << std::endl;
                    }

                    int frame = process_table[current_process].page_table[i].physical_frame;
                    frame_table[frame].first = -1;
                    free_list.push_back(frame);
                }
            }
            continue;
        }

        access_count++;

        PageTableEntry *pte = &process_table[current_process].page_table[vpage];

        // if PTE valid
        if (pte->present == 1)
        {
            pte->referenced = 1;
            if (operation == 'w')
            {
                // check write protection
                if (pte->write_protect)
                {
                    process_table[current_process].segprot++;
                    std::cout << " SEGPROT" << std::endl;
                }
                else
                {
                    pte->modified = 1;
                }
            }
            continue;
        }

        // if PTE not valid, page fault handler
        // firstly detect segv
        Vma *vma = process_table[current_process].get_vma(vpage);
        if (vma == NULL)
        {
            process_table[current_process].segv++;
            std::cout << " SEGV" << std::endl;
            continue;
        }
        else
        {
            pte->filemapped = vma->filemapped;
            pte->write_protect = vma->write_protected;
        }

        // instantiate the page
        int frame = get_frame();
        pte->present = 1;
        pte->physical_frame = frame;

        // populate the page content, two situations for map, 1. the page has never been used before 2. the page has been paged out before
        if (pte->paged_out == 0)
        {
            if (pte->filemapped == 1)
            {
                process_table[current_process].fins++;
                std::cout << " FIN" << std::endl;
            }
            else
            {
                process_table[current_process].zeros++;
                std::cout << " ZERO" << std::endl;
            }
        }
        else
        {
            if (pte->filemapped == 1)
            {
                process_table[current_process].fins++;
                std::cout << " FIN" << std::endl;
            }
            else
            {
                process_table[current_process].ins++;
                std::cout << " IN" << std::endl;
            }
        }

        // reverse mapping
        frame_table[frame].first = current_process;
        frame_table[frame].second = vpage;
        process_table[current_process].maps++;
        time_last_used_table[frame] = current_instr;
        age_table[frame] = 0;
        std::cout << " MAP " << frame << std::endl;

        // check write protection and update
        pte->referenced = 1;
        if (operation == 'w')
        {
            // check write protection
            if (pte->write_protect)
            {
                process_table[current_process].segprot++;
                std::cout << " SEGPROT" << std::endl;
            }
            else
            {
                pte->modified = 1;
            }
        }
    }
    print_page_tables();
    print_frame_table();
    print_process_table();
    print_summary_info();
}