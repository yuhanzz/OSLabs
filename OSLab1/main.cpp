#include <iostream>
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

int getOperand(char type, int baseAddress, std::list<std::string> &useList, int operand)
{
    int modifiedOperand;
    switch (type)
    {
    case 'R':
        modifiedOperand = baseAddress + operand;
        break;
    case 'E':
    { // need use list
        std::list<std::string>::iterator it = useList.begin();
        std::advance(it, operand);
        modifiedOperand = symbolTable.getLocation(*it);
        break;
    }
    case 'A':
        if (operand >= 512)
        {
            moduleWarnings.push_back(" >= machine size");
        }
        modifiedOperand = operand;
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

    while (tokenizer.readDefCount(defCount))
    {
        // module related variables;
        std::list<std::string> useList;

        int useCount, instCount;

        for (int i = 0; i < defCount; i++)
        {
            std::string symbolName = tokenizer.readSymbol();
            int relativeAddress = tokenizer.readAddress();
            symbolTable.createSymbol(symbolName, relativeAddress + baseAddress);
        }

        useCount = tokenizer.readUseCount();

        for (int i = 0; i < useCount; i++)
        {
            std::string symbol = tokenizer.readSymbol();
            // do some check
        }

        instCount = tokenizer.readInstCount(totalInstCount);
        for (int i = 0; i < instCount; i++)
        {
            char addressMode = tokenizer.readIEAR();
            int operand = tokenizer.readOperand();
        }

        // update base address for each module
        baseAddress += instCount;
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

    while (tokenizer.readDefCount(defCount))
    {
        // module related variables;
        std::list<std::string> useList;
        moduleWarnings.clear();
        int useCount, instCount;

        // read def list
        for (int i = 0; i < defCount; i++)
        {
            std::string symbolName = tokenizer.readSymbol();
            int relativeAddress = tokenizer.readAddress();
            symbolTable.createSymbol(symbolName, relativeAddress + baseAddress);
        }

        useCount = tokenizer.readUseCount();

        for (int i = 0; i < useCount; i++)
        {
            std::string symbol = tokenizer.readSymbol();
            // do some check
        }

        instCount = tokenizer.readInstCount(totalInstCount);
        for (int i = 0; i < instCount; i++)
        {
            char addressMode = tokenizer.readIEAR();
            int operand = tokenizer.readOperand();
        }

        baseAddress += instCount;
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

    try
    {
        pass2();
    }
    catch (std::exception &e)
    {
        std::cout << e.what();
    }

    infile.close();
    infile.clear();

    return 0;
}
