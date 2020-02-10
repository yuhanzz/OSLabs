#include <iostream>
#include <string>
#include <list>

extern std::list<std::string> globalWarinigs;

enum Pass2ErrorCode
{
    NOT_DEFINED = -1,
    EXCEEDS_512 = -2

};

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
    bool used;
    int module;

    Symbol(std::string, int, int);
};

class SymbolTable
{
public:
    std::list<Symbol> table;
    void createSymbol(std::string, int, int);
    int getLocation(std::string);
    void print();
    void printUnused();
};