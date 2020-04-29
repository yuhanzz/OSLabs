#ifndef PROCESS_H_
#define PROCESS_H_

#include <iostream>
#include <list>

#define VIRTUAL_PAGE_COUNT 64

class Vma
{
public:
    int starting_virtual_page;
    int ending_virtual_page;
    int write_protected;
    int filemapped;
    Vma(int starting_virtual_page, int ending_virtual_page, int write_protected, int filemapped)
    {
        this->starting_virtual_page = starting_virtual_page;
        this->ending_virtual_page = ending_virtual_page;
        this->write_protected = write_protected;
        this->filemapped = filemapped;
    }
};


struct PageTableEntry
{
    unsigned present : 1;
    unsigned write_protect : 1;
    unsigned filemapped : 1;
    unsigned modified : 1;
    unsigned referenced : 1;
    unsigned paged_out : 1;
    unsigned physical_frame : 26;

    PageTableEntry() {
        this->present = 0;
        this->modified = 0;
        this->referenced = 0;
        this->paged_out = 0;
    }
};

class Process
{
public:
    int id;
    std::list<Vma *> vma_list;
    struct PageTableEntry page_table[VIRTUAL_PAGE_COUNT];

    // summary
    unsigned long long unmaps;
    unsigned long long maps;
    unsigned long long ins;
    unsigned long long outs;
    unsigned long long fins;
    unsigned long long fouts;
    unsigned long long zeros;
    unsigned long long segv;
    unsigned long long segprot;

    Process()
    {
        unmaps = 0;
        maps = 0;
        ins = 0;
        outs = 0;
        fins = 0;
        fouts = 0;
        zeros = 0;
        segv = 0;
        segprot = 0;
    }

    void print_page_table();
    Vma* get_vma(int);
};

#endif