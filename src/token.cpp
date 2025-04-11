#include "token.hpp"

#include <iostream>

void Token::print() const
{
  std::cout << "Token(" << tokenTypeToString(type)
    << ", \"" << value << "\", "
    << row << column << ")" << std::endl;
}

std::string Token::tokenTypeToString(TokenType type) const
{
  switch (type) 
  {
    case TokenType::NUMBER: return "NUMBER";
    case TokenType::FLOAT: return "FLOAT";
    case TokenType::STRING_LITERAL: return "STRING_LITERAL";
    case TokenType::CHAR_LITERAL: return "CHAR_LITERAL";
    case TokenType::BOOLEAN_LITERAL: return "BOOLEAN_LITERAL";
    case TokenType::IDENTIFIER: return "IDENTIFIER";
    case TokenType::KEYWORD: return "KEYWORD";
    case TokenType::PLUS: return "PLUS";
    case TokenType::MINUS: return "MINUS";
    case TokenType::MUL: return "MUL";
    case TokenType::DIV: return "DIV";
    case TokenType::MOD: return "MOD";
    case TokenType::ASSIGN: return "ASSIGN";
    case TokenType::EQ: return "EQ";
    case TokenType::NEQ: return "NEQ";
    case TokenType::GT: return "GT";
    case TokenType::LT: return "LT";
    case TokenType::GTE: return "GTE";
    case TokenType::LTE: return "LTE";
    case TokenType::PLUS_ASSIGN: return "PLUS_ASSIGN";
    case TokenType::MINUS_ASSIGN: return "MINUS_ASSIGN";
    case TokenType::MUL_ASSIGN: return "MUL_ASSIGN";
    case TokenType::DIV_ASSIGN: return "DIV_ASSIGN";
    case TokenType::MOD_ASSIGN: return "MOD_ASSIGN";
    case TokenType::AND: return "AND";
    case TokenType::OR: return "OR";
    case TokenType::NOT: return "NOT";
    case TokenType::SEMICOLON: return "SEMICOLON";
    case TokenType::COMMA: return "COMMA";
    case TokenType::DOT: return "DOT";
    case TokenType::COLON: return "COLON";
    case TokenType::LPAREN: return "LPAREN";
    case TokenType::RPAREN: return "RPAREN";
    case TokenType::LBRACE: return "LBRACE";
    case TokenType::RBRACE: return "RBRACE";
    case TokenType::LBRACKET: return "LBRACKET";
    case TokenType::RBRACKET: return "RBRACKET";
    case TokenType::END_OF_FILE: return "EOF";
    case TokenType::INVALID: return "INVALID";
    default: return "UNKNOWN";
  }
}
