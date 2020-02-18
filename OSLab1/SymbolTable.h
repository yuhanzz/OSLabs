#include <iostream>
#include <string>
#include <list>
#include <unordered_map>

class Symbol
{
public:
    std::string name;
    int address;

    Symbol(std::string, int);
};

class SymbolTable
{
public:
    std::list<Symbol> table;
    int createSymbol(std::string, int);
    int getLocation(std::string);
    void print(std::unordered_map<std::string, int> &);
};