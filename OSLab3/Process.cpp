#include "Process.h"

void Process::print_page_table()
{
    for (int i = 0; i < VIRTUAL_PAGE_COUNT; i++)
    {
        if (page_table[i].present == 1)
        {
            std::cout << i << ":";
            if (page_table[i].referenced == 1)
            {
                std::cout << "R";
            }
            else
            {
                std::cout << "-";
            }
            if (page_table[i].modified == 1)
            {
                std::cout << "M";
            }
            else
            {
                std::cout << "-";
            }
            if (page_table[i].paged_out == 1)
            {
                std::cout << "S";
            }
            else
            {
                std::cout << "-";
            }
        }
        else
        {
            if (page_table[i].paged_out == 1)
            {
                std::cout << "#";
            }
            else
            {
                std::cout << "*";
            }
        }
        std::cout << " ";
    }
    std::cout << std::endl;
}

Vma *Process::get_vma(int vpage)
{
    std::list<Vma *>::iterator iter;
    for (iter = vma_list.begin(); iter != vma_list.end(); iter++)
    {
        if ((*iter)->starting_virtual_page <= vpage && vpage <= (*iter)->ending_virtual_page)
        {
            return (*iter);
        }
    }
    return NULL;
}