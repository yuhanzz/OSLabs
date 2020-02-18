#include <iostream>
#include <string>
#include "SymbolTable.h"

Symbol::Symbol(std::string name, int address)
{
    // deep copy or shallow copy ???
    this->name = name;
    this->address = address;
}

int SymbolTable::createSymbol(std::string symbol, int address)
{
    for (std::list<Symbol>::iterator it = table.begin(); it != table.end(); it++)
    {
        if (symbol.compare(it->name) == 0)
        {
            // defined multiple times
            return -1;
        }
    }
    Symbol s(symbol, address);
    table.push_back(s);
    return 1;
}

int SymbolTable::getLocation(std::string symbol)
{
    for (std::list<Symbol>::iterator it = table.begin(); it != table.end(); it++)
    {
        if (symbol.compare(it->name) == 0)
        {
            return it->address;
        }
    }

    return -1;
}

void SymbolTable::print(std::unordered_map<std::string, int> &duplicateDefineTable)
{
    std::cout << "Symbol Table" << std::endl;

    for (std::list<Symbol>::const_iterator it = table.begin(); it != table.end(); it++)
    {
        std::cout << it->name << '=' << it->address;

        // rule 2
        if (duplicateDefineTable.find(it->name) != duplicateDefineTable.end())
        {
            std::cout << " Error: This variable is multiple times defined; first value used";
        }

        std::cout << std::endl;
    }
}
