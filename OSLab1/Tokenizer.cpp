#include <iostream>
#include <fstream>
#include <cstring>
#include <regex>
#include "Tokenizer.h"

Token::Token(int lineNumber, int lineOffset, std::string token)
{
    this->lineNumber = lineNumber;
    this->lineOffset = lineOffset;
    this->token = token;
}

bool Token::isEmpty()
{
    return token.compare("") == 0;
}

Tokenizer::Tokenizer()
{
    currentLineNumber = 0;
    currentLine = NULL;
    currentLineLength = 0;
};

bool Tokenizer::getLine()
{
    std::string line;

    if (!infile.eof())
    {
        std::getline(infile, line);

        if (line.length() == 0 && infile.eof())
        {
            // if the file ends with \n, don't update current line number and current line length
            return false;
        }

        currentLineNumber++;
        currentLineLength = line.length();
        currentLine = new char[currentLineLength + 1];
        strcpy(currentLine, line.c_str());

        return true;
    }
    else
    {
        return false;
    }
}

Token Tokenizer::getToken()
{
    const char *delim = " \t";
    char *token = NULL;
    while (token == NULL)
    {
        // firstly, try to strtok from currentLine
        token = strtok(NULL, delim);
        // if NULL, getLine
        if (token == NULL)
        {
            if (!getLine())
            {
                // no more lines to get
                return Token(currentLineNumber, currentLineLength + 1, "");
            }
            else
            {
                token = strtok(currentLine, delim);
            }
        }
    }
    return Token(currentLineNumber, token - currentLine + 1, std::string(token));
}

int Tokenizer::readAddress()
{
    Token token = getToken();
    if (token.isEmpty())
    {
        throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
    }
    try
    {
        int address = std::stoi(token.token);
        if (address < 0) 
        {
            throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
        }
        return address;
    }
    catch (std::invalid_argument &e)
    {
        throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
    }
    catch (std::out_of_range &e)
    {
        if (token.token.at(0) == '-')
        {
            throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
        }
        else
        {
            // if the number is larger than integer, it will be caught and treated as relative address zero in pass2
            return 10000;
        }
    }
}

int Tokenizer::readUseCount()
{
    Token token = getToken();
    if (token.isEmpty())
    {
        throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
    }
    try
    {
        int count = std::stoi(token.token);
        if (count > 16)
        {
            throw SyntaxError(TOO_MANY_USE_IN_MODULE, token.lineNumber, token.lineOffset);
        }
        if (count < 0) {
            throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
        }
        return count;
    }
    catch (std::invalid_argument &e)
    {
        throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
    }
    catch (std::out_of_range &e)
    {
        if (token.token.at(0) == '-')
        {
            throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
        }
        else
        {
            throw SyntaxError(TOO_MANY_USE_IN_MODULE, token.lineNumber, token.lineOffset);
        }
    }
}

int Tokenizer::readInstCount(int &totalInstCount)
{
    Token token = getToken();
    if (token.isEmpty())
    {
        throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
    }
    try
    {
        int count = std::stoi(token.token);
        if (count < 0) {
            throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
        }

        totalInstCount += count;
        if (totalInstCount > 512)
        {
            throw SyntaxError(TOO_MANY_INSTR, token.lineNumber, token.lineOffset);
        }
        return count;
    }
    catch (std::invalid_argument &e)
    {
        throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
    }
    catch (std::out_of_range &e)
    {
        if (token.token.at(0) == '-')
        {
            throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
        }
        else
        {
            throw SyntaxError(TOO_MANY_INSTR, token.lineNumber, token.lineOffset);
        }
    }
}

char Tokenizer::readIEAR()
{
    Token token = getToken();
    if (token.isEmpty())
    {
        throw SyntaxError(ADDR_EXPECTED, token.lineNumber, token.lineOffset);
    }

    if (token.token.length() == 1)
    {
        char addr[2];
        strcpy(addr, token.token.c_str());
        if (addr[0] == 'I' || addr[0] == 'E' || addr[0] == 'A' || addr[0] == 'R')
        {
            return addr[0];
        }
        else
        {
            throw SyntaxError(ADDR_EXPECTED, token.lineNumber, token.lineOffset);
        }
    }
    else
    {
        throw SyntaxError(ADDR_EXPECTED, token.lineNumber, token.lineOffset);
    }
}

int Tokenizer::readOperand()
{

    Token token = getToken();
    if (token.isEmpty())
    {
        throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
    }
    try
    {
        int operand = std::stoi(token.token);
        if (operand < 0) 
        {
            throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
        }
        return operand;
    }
    catch (std::invalid_argument &e)
    {
        throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
    }
    catch (std::out_of_range &e)
    {
        if (token.token.at(0) == '-')
        {
            throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
        }
        else
        {
            return 10000;
        }
    }
}

std::string Tokenizer::readSymbol()
{
    Token token = getToken();
    if (token.isEmpty())
    {
        throw SyntaxError(SYM_EXPECTED, token.lineNumber, token.lineOffset);
    }
    if (token.token.length() > 16)
    {
        throw SyntaxError(SYM_TOO_LONG, token.lineNumber, token.lineOffset);
    }
    if (std::regex_match(token.token, std::regex("[A-Za-z][A-Za-z0-9]*")))
    {
        return token.token;
    }
    else
    {
        throw SyntaxError(SYM_EXPECTED, token.lineNumber, token.lineOffset);
    }
}

bool Tokenizer::readDefCount(int &defcount)
{
    Token token = getToken();
    if (token.isEmpty())
    {
        return false;
    }
    else
    {
        try
        {
            defcount = std::stoi(token.token);
            if (defcount > 16)
            {
                throw SyntaxError(TOO_MANY_DEF_IN_MODULE, token.lineNumber, token.lineOffset);
            }
            if (defcount < 0) 
            {
                throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
            }
        }
        catch (std::invalid_argument &e)
        {
            throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
        }
        catch (std::out_of_range &e)
        {
            if (token.token.at(0) == '-')
            {
                throw SyntaxError(NUM_EXPECTED, token.lineNumber, token.lineOffset);
            }
            else
            {
                throw SyntaxError(TOO_MANY_DEF_IN_MODULE, token.lineNumber, token.lineOffset);
            }
        }

        return true;
    }
}