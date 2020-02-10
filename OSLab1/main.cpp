#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include "Tokenizer.h"
#include "SymbolTable.h"

std::ifstream infile;
SymbolTable symbolTable;
Tokenizer tokenizer;
// put the warning list inside the functions when necessary
std::list<std::string> globalWarnings;
std::list<std::string> moduleWarnings;

// class WarningException : public std::exception
// {

// public:
//     std::string message;
//     WarningException(std::string const &message)
//     {
//         this->message = message;
//     }
//     // const char *what() const noexcept override { return message.c_str(); }
//     const char* what() const noexcept {return "Ooops!\n";}
// };

// 如果出现了一个在symbol table之前的warning，然后又出现了一个syntax error,那那个warning应不应该打印

// the number of total instructions should be less than 512

// 如果读到了文件的结尾，返回一个null 的token，

// try
// {
//     int count = std::stoi(token.token);

//     totalInstCount += count;
//     if (totalInstCount > 512) {
//         throw SyntaxError(TOO_MANY_INSTR, token.lineNumber, token.lineOffset);
//     }
//     return count;
// }
// catch (std::exception &e)
// {
//     throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
// }

//谨防自己抛出的syntax error 被自己写的catch catch到的情况

int getOperand(char type, int baseAddress, std::list< std::pair<std::string, bool> > &useList, int operand, std::string &errorSymbol)
{
    int modifiedOperand;
    switch (type)
    {
    case 'R':
        modifiedOperand = baseAddress + operand;
        break;
    case 'E':
    { // need use list
        std::list< std::pair<std::string, bool> >::iterator it = useList.begin();
        std::advance(it, operand);
        modifiedOperand = symbolTable.getLocation(it->first);
        if (modifiedOperand == NOT_DEFINED)
        {
            errorSymbol = it->first;
        }
        it->second = true;
        break;
    }
    case 'A':
        if (operand >= 512)
        {
            modifiedOperand = EXCEEDS_512;
        }
        else
        {
            modifiedOperand = operand;
        }
        break;
    case 'I':
        modifiedOperand = operand;
        break;
    }

    return modifiedOperand;
}

void pass1()
{
    // file related variables
    int totalInstCount = 0;
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
            std::string symbolName = tokenizer.readSymbol();
            int relativeAddress = tokenizer.readAddress();
            defList.push_back(Symbol(symbolName, relativeAddress, moduleIndex));
        }

        useCount = tokenizer.readUseCount();

        for (int i = 0; i < useCount; i++)
        {
            tokenizer.readSymbol();
            // do some check
        }

        instCount = tokenizer.readInstCount(totalInstCount);

        // insert into symbol table
        for (std::list<Symbol>::iterator iter = defList.begin(); iter != defList.end(); iter++)
        {
            int address;
            if (iter->address >= instCount)
            {
                std::cout << "Warning: Module " << iter->module << ": " << iter->name << " too big " << iter->address << " (max=" << instCount - 1 << ") assume zero relative" << std::endl;
                address = baseAddress;
            }
            else
            {
                address = baseAddress + iter->address;
            }
            symbolTable.createSymbol(iter->name, address, iter->module);
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
}

// main function for pass two
void pass2()
{
    // file related variables
    int totalInstCount = 0;
    // module related variables;
    int defCount;
    int baseAddress = 0;

    int instructionIndex = 0, moduleIndex = 1;

    while (tokenizer.readDefCount(defCount))
    {
        // module related variables;
        std::list< std::pair<std::string, bool> > useList;
        moduleWarnings.clear();
        int useCount, instCount;

        // read def list
        for (int i = 0; i < defCount; i++)
        {
            // readSymbol function can't print out anything, or the warning will duplicate
            tokenizer.readSymbol();
            tokenizer.readAddress();
        }

        useCount = tokenizer.readUseCount();

        for (int i = 0; i < useCount; i++)
        {
            std::string symbol = tokenizer.readSymbol();
            useList.push_back(std::pair<std::string, bool>(symbol, false));
            // do some check
        }

        instCount = tokenizer.readInstCount(totalInstCount);
        for (int i = 0; i < instCount; i++)
        {
            char addressMode = tokenizer.readIEAR();
            int op = tokenizer.readOperand();
            std::string errorSymbol;

            int address = getOperand(addressMode, baseAddress, useList, op % 1000, errorSymbol);

            // print instructions and following errors
            std::cout << std::setfill('0') << std::setw(3) << instructionIndex++ << ": ";
            std::cout << op / 1000;

            if (address < 0)
            {
                switch (address)
                {
                case NOT_DEFINED:
                {
                    std::cout << std::setfill('0') << std::setw(3) << 0;
                    std::cout << " Error: " << errorSymbol << " is not defined; zero used";
                    break;
                }
                case EXCEEDS_512:
                {
                    std::cout << std::setfill('0') << std::setw(3) << 0;
                    std::cout << " Error: Absolute address exceeds machine size; zero used";
                    break;
                }
                }
            }
            else
            {
                std::cout << std::setfill('0') << std::setw(3) << address;
            }

            std::cout << std::endl;
        }

        baseAddress += instCount;

        // print out rule 7, appear in use list but not used in E-type
        for (std::list< std::pair<std::string, bool> >::iterator iter = useList.begin(); iter != useList.end(); iter++)
        {
            if (iter->second == false)
            {
                std::cout << "Warning: Module " << moduleIndex << ": " << iter->first << " appeared in the uselist but was not actually used" << std::endl;
            }
        }

        moduleIndex++;
    }

    // should we put it in here???
    std::cout << std::endl;
    // print out rule 4, defined but not used
    symbolTable.printUnused();
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

    // print some warnings

    // print symbol table and multiple defined values
    symbolTable.print();

    infile.clear();
    infile.seekg(0, std::ios::beg);

    try
    {
        // where to put this ?
        std::cout << "\nMemory Map\n";
        pass2();
    }
    catch (std::exception &e)
    {
        std::cout << e.what();
    }

    infile.close();

    return 0;
}
