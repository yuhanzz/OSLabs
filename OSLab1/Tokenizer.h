#include <iostream>
#include <fstream>
#include <string>

extern std::ifstream infile;

// use linked list

enum ErrorCode
{
    NUM_EXPECTED = 0,
    SYM_EXPECTED,
    ADDR_EXPECTED,
    SYM_TOO_LONG,
    TOO_MANY_DEF_IN_MODULE,
    TOO_MANY_USE_IN_MODULE,
    TOO_MANY_INSTR
};

class SyntaxError : public std::exception
{
public:
    SyntaxError(ErrorCode errorCode, int lineNumber, int lineOffset)
    {

        std::string errstr[] = {
            "NUM_EXPECTED",           // Number expect
            "SYM_EXPECTED",           // Symbol Expected
            "ADDR_EXPECTED",          // Addressing Expected which is A/E/I/R
            "SYM_TOO_LONG",           // Symbol Name is too long
            "TOO_MANY_DEF_IN_MODULE", // > 16
            "TOO_MANY_USE_IN_MODULE", // > 16
            "TOO_MANY_INSTR",         // total num_instr exceeds memory size (512)
        };

        std::cout << "Parse Error line " << lineNumber << " offset " << lineOffset << ": " << errstr[errorCode] << "\n";
    }
};

class Token
{
public:
    int lineNumber;
    int lineOffset;
    std::string token;
    Token(int lineNumber, int lineOffset, std::string token);
    bool isEmpty();
};

class Tokenizer
{
public:
    int currentLineNumber;
    char *currentLine;
    int currentLineLength;

    Tokenizer();
    Token getToken();
    bool getLine();
    bool readDefCount(int &);

    // illegal check: A-z, if illegal, throw an exception
    //  SYM_EXPECTED: Symbols always begin with alpha characters followed by optional alphanumerical characters, i.e.[a-Z][a-Z0-9]*.
    // SYM_TOO_LONG: Valid symbols can be up to 16 characters.
    std::string readSymbol();
    int readAddress();
    int readUseCount();
    int readInstCount(int&);
    char readIEAR();
    int readOperand();
};