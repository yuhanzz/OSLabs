#include <iostream>
#include <string>
#include <list>



class DuplicateSymbolException : public std::exception
{
public:
    const char *what() const throw()
    {
        return "Error: This variable is multiple times defined; first value used";
    }
};

class Symbol
{
public:
    std::string name;
    int address;
    bool redefined;

    Symbol(std::string, int);
};

class SymbolTable
{
public:
    std::list<Symbol> table;
    bool createSymbol(std::string, int);
    int getLocation(std::string);
    void print();
};