#include <iostream>
#include <string>
#include "SymbolTable.h"

Symbol::Symbol(std::string name, int address, int module)
{
    // deep copy or shallow copy ???
    this->name = name;
    this->address = address;
    this->redefined = false;
    this->used = false;
    this->module = module;
}

void SymbolTable::createSymbol(std::string symbol, int address, int module)
{
    // do some check

    // legal symbol check in readSymbol function

    // if duplicate symbol found
    bool duplicate = false;
    for (std::list<Symbol>::iterator it = table.begin(); it != table.end(); it++)
    {
        if (symbol.compare(it->name) == 0)
        {
            // print out message instead of throw the exception
            it->redefined = true;
            duplicate = true;
        }
    }

    // if address is more than the legal range
    if (!duplicate)
    {
        Symbol s(symbol, address, module);
        table.push_back(s);
    }
}

int SymbolTable::getLocation(std::string symbol)
{
    for (std::list<Symbol>::iterator it = table.begin(); it != table.end(); it++)
    {
        if (symbol.compare(it->name) == 0)
        {
            it->used = true;
            return it->address;
        }
    }

    // if -1, it means that the symbol is not defined
    return NOT_DEFINED;
}

void SymbolTable::print()
{
    std::cout << "Symbol Table" << std::endl;

    for (std::list<Symbol>::const_iterator it = table.begin(); it != table.end(); it++)
    {
        std::cout << it->name << '=' << it->address;
        if (it->redefined)
        {
            std::cout << " Error: This variable is multiple times defined; first value used";
        }
        std::cout << std::endl;
    }
}

void SymbolTable::printUnused() 
{
    for (std::list<Symbol>::const_iterator it = table.begin(); it != table.end(); it++)
    {
        if (!it->used)
        {
            std::cout << "Warning: Module " << it->module << ": " << it->name << " was defined but never used" << std::endl;
        }
    }
}