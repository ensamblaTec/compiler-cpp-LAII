#pragma once

#include "token.hpp"

#include <iostream>
#include <string>

class Lexer
{
  public:
    Lexer(std::istream& input);
    Token nextToken();

  private:
    std::istream& input;
    std::string currentLine;
    size_t pos = 0;
    int row = 0;
    int column = 1;

    void readNextLine();
};
