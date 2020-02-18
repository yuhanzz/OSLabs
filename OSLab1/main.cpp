#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <unordered_map>
#include "Tokenizer.h"
#include "SymbolTable.h"

std::ifstream infile;
SymbolTable symbolTable;
Tokenizer tokenizer;

void printInstruction(char type, int baseAddress, int moduleSize, std::unordered_map<std::string, bool> &defineUseMap, std::unordered_map<std::string, bool> &useListUseMap, std::list<std::string> &useList, int op, int instructionIndex)
{
    std::cout << std::setfill('0') << std::setw(3) << instructionIndex << ": ";

    // rule 11
    if (type != 'I' && op > 9999)
    {
        std::cout << 9999;
        std::cout << " Error: Illegal opcode; treated as 9999" << std::endl;
        return;
    }

    int operand = op % 1000;

    switch (type)
    {
    case 'R':
    {
        if (operand < moduleSize)
        {
            std::cout << op / 1000;
            std::cout << std::setfill('0') << std::setw(3) << baseAddress + operand << std::endl;
        }
        else
        {
            // rule 9
            std::cout << op / 1000;
            std::cout << std::setfill('0') << std::setw(3) << baseAddress << " Error: Relative address exceeds module size; zero used" << std::endl;
        }
        return;
    }
    case 'E':
    {
        if (operand < useList.size())
        {
            std::list<std::string>::iterator it = useList.begin();
            std::advance(it, operand);
            int position = symbolTable.getLocation(*it);
            if (position == -1)
            {
                // rule 3
                std::cout << op / 1000;
                std::cout << std::setfill('0') << std::setw(3) << 0 << " Error: " << *it << " is not defined; zero used" << std::endl;
            }
            else
            {
                std::cout << op / 1000;
                std::cout << std::setfill('0') << std::setw(3) << position << std::endl;
            }

            defineUseMap[*it] = true;
            useListUseMap[*it] = true;
        }
        else
        {
            // rule 6
            std::cout << op / 1000;
            std::cout << std::setfill('0') << std::setw(3) << operand << " Error: External address exceeds length of uselist; treated as immediate" << std::endl;
        }
        return;
    }
    case 'A':
        if (operand >= 512)
        {
            // rule 8
            std::cout << op / 1000;
            std::cout << std::setfill('0') << std::setw(3) << 0 << " Error: Absolute address exceeds machine size; zero used" << std::endl;
        }
        else
        {
            std::cout << op / 1000;
            std::cout << std::setfill('0') << std::setw(3) << operand << std::endl;
        }
        return;
    case 'I':
        if (op / 1000 >= 10)
        {
            // rule 10
            std::cout << 9999;
            std::cout << " Error: Illegal immediate value; treated as 9999" << std::endl;
        }
        else
        {
            std::cout << op / 1000;
            std::cout << std::setfill('0') << std::setw(3) << operand << std::endl;
        }
        return;
    }
}

void pass1()
{
    // file related variables
    int totalInstCount = 0;
    std::unordered_map<std::string, int> duplicateDefineTable; // (symbol, 2) can be found if the symbols are defined multiple times
    std::unordered_map<std::string, int> moduleTable;          // record the module where the symbols are defined
    // module related variables;
    int defCount;
    int baseAddress = 0;
    int moduleIndex = 1;

    while (tokenizer.readDefCount(defCount))
    {
        std::list<Symbol> defList;
        int useCount, instCount;

        for (int i = 0; i < defCount; i++)
        {
            // store the definition list and insert later (after knowing the module size)
            std::string symbolName = tokenizer.readSymbol();
            int relativeAddress = tokenizer.readAddress();
            defList.push_back(Symbol(symbolName, relativeAddress));
        }

        useCount = tokenizer.readUseCount();

        for (int i = 0; i < useCount; i++)
        {
            tokenizer.readSymbol();
        }

        instCount = tokenizer.readInstCount(totalInstCount);

        // insert into symbol table
        for (std::list<Symbol>::iterator iter = defList.begin(); iter != defList.end(); iter++)
        {
            bool tooBig = false;
            int address;
            if (iter->address >= instCount)
            {
                address = baseAddress;
                tooBig = true;
            }
            else
            {
                address = baseAddress + iter->address;
            }

            if (symbolTable.createSymbol(iter->name, address) == -1)
            {
                duplicateDefineTable[iter->name] = 2;
            }
            else
            {
                moduleTable[iter->name] = moduleIndex;
                if (tooBig)
                {
                    std::cout << "Warning: Module " << moduleTable[iter->name] << ": " << iter->name << " too big " << iter->address << " (max=" << instCount - 1 << ") assume zero relative" << std::endl;
                }
            }
        }

        for (int i = 0; i < instCount; i++)
        {
            tokenizer.readIEAR();
            tokenizer.readOperand();
        }

        // update base address for each module
        baseAddress += instCount;

        moduleIndex++;
    }

    // print symbol table and multiple defined values
    symbolTable.print(duplicateDefineTable);
}

// main function for pass two
void pass2()
{
    std::cout << "\nMemory Map\n";

    // file related variables
    int totalInstCount = 0;
    std::unordered_map<std::string, int> moduleTable; // record the module where the symbols are defined
    std::unordered_map<std::string, bool> defineUseMap;
    int instructionIndex = 0, moduleIndex = 1;

    // module related variables;
    int defCount;
    int baseAddress = 0;

    while (tokenizer.readDefCount(defCount))
    {
        std::unordered_map<std::string, bool> useListUseMap;
        std::list<std::string> useList;
        int useCount, instCount;

        // read def list
        for (int i = 0; i < defCount; i++)
        {
            std::string symbol = tokenizer.readSymbol();

            if (moduleTable.find(symbol) == moduleTable.end())
            {
                // record the module where the symbol is defined for the first time
                moduleTable[symbol] = moduleIndex;
            }

            tokenizer.readAddress();
        }

        useCount = tokenizer.readUseCount();

        for (int i = 0; i < useCount; i++)
        {
            std::string symbol = tokenizer.readSymbol();
            useListUseMap[symbol] = false;
            useList.push_back(symbol);
        }

        instCount = tokenizer.readInstCount(totalInstCount);
        for (int i = 0; i < instCount; i++)
        {
            char addressMode = tokenizer.readIEAR();
            int op = tokenizer.readOperand();

            printInstruction(addressMode, baseAddress, instCount, defineUseMap, useListUseMap, useList, op, instructionIndex++);
        }

        baseAddress += instCount;

        // rule 7
        for (std::list<std::string>::iterator iter = useList.begin(); iter != useList.end(); iter++)
        {
            if (useListUseMap[*iter] == false)
            {
                std::cout << "Warning: Module " << moduleIndex << ": " << *iter << " appeared in the uselist but was not actually used" << std::endl;
            }
        }
        moduleIndex++;
    }

    std::cout << std::endl;

    // rule 4
    for (std::list<Symbol>::iterator iter = symbolTable.table.begin(); iter != symbolTable.table.end(); iter++)
    {
        if (defineUseMap.find(iter->name) == defineUseMap.end())
        {
            std::cout << "Warning: Module " << moduleTable[iter->name] << ": " << iter->name << " was defined but never used" << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Please enter one file name";
        return 0;
    }
    std::string fileName = argv[1];

    infile.open(fileName, std::ios::in);

    try
    {
        pass1();
    }
    catch (SyntaxError &e)
    {
        return 0;
    }

    infile.clear();
    infile.seekg(0, std::ios::beg);

    pass2();

    infile.close();

    return 0;
}
