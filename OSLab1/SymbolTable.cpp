#include <iostream>
#include <string>
#include "SymbolTable.h"

Symbol::Symbol(std::string name, int address)
{
    // deep copy or shallow copy ???
    this->name = name;
    this->address = address;
    this->redefined = false;
}

bool SymbolTable::createSymbol(std::string symbol, int address)
{
    // do some check

    // legal symbol check in readSymbol function

    // if duplicate symbol found
    for (std::list<Symbol>::iterator it = table.begin(); it != table.end(); it++)
    {
        if (symbol.compare(it->name) == 0)
        {
            // print out message instead of throw the exception
            it->redefined = true;
        }
    }

    // if address is more than the legal range

    Symbol s(symbol, address);
    table.push_back(s);

    return true;
}

int SymbolTable::getLocation(std::string symbol)
{
    for (std::list<Symbol>::const_iterator it = table.begin(); it != table.end(); it++)
    {
        if (symbol.compare(it->name) == 0)
        {
            return it->address;
        }
    }
    // for solving warning ?
    return 0;
}

void SymbolTable::print() {
    for (std::list<Symbol>::const_iterator it = table.begin(); it != table.end(); it++) {
        std::cout << it->name << '=' << it->address;
        if (it->redefined) {
            std::cout << " Error: This variable is multiple times defined; first value used";
        }
    }
}