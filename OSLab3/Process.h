#include <iostream>
#include <list>

class Vma {
    public:
    int starting_virtual_page;
    int ending_virtual_page;
    int write_protected;
    int filemapped;
    Vma(int starting_virtual_page, int ending_virtual_page, int write_protected, int filemapped) {
        this->starting_virtual_page = starting_virtual_page;
        this->ending_virtual_page = ending_virtual_page;
        this->write_protected = write_protected;
        this->filemapped = filemapped;
    }
};

class Process {
    public:
    int id;
    std::list<Vma*> vma_list;
};