#include <iostream>
#include "utils.hpp"
#include "logger.hpp"

std::vector<Token> getAllTokens(Lexer& lexer)
{
  Token token;
  std::vector<Token> tokens;

  while ((token = lexer.nextToken()).type != TokenType::END_OF_FILE) {
    if (token.type == TokenType::INVALID)
    {
      LOG(LogLevel::ERROR, "Error lexico en linea: " + std::to_string(token.row) 
          + ", columna: " + std::to_string(token.column) 
          + ", caracter invalido '" + token.value
      );
      continue;
    }
    tokens.push_back(token);
  }

  tokens.push_back(token);

  return tokens;
}

