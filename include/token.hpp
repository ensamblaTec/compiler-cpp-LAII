#pragma once

#include <string>
#include <iostream>

enum class TokenType
{
  NUMBER,
  FLOAT,
  STRING_LITERAL,
  CHAR_LITERAL,
  BOOLEAN_LITERAL,
  IDENTIFIER,
  KEYWORD_INT,
  KEYWORD_STR,
  KEYWORD_FLOAT,
  KEYWORD_BOOL,
  KEYWORD_IF,
  KEYWORD_ELSE,
  KEYWORD_WHILE,
  KEYWORD_FOR,
  KEYWORD_PRINT,
  KEYWORD_INPUT,
  PLUS, MINUS, MUL, DIV, MOD,
  ASSIGN,
  EQ, NEQ, GT, LT, GTE, LTE,
  PLUS_ASSIGN, MINUS_ASSIGN, MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN,
  AND, OR, NOT,
  SEMICOLON, COMMA, DOT, COLON, SLASH, STAR,
  LPAREN, RPAREN,
  LBRACE, RBRACE,
  LBRACKET, RBRACKET,
  END_OF_FILE,
  INVALID
};

inline std::string tokenTypeToString(TokenType type)
{
  switch (type) 
  {
    case TokenType::NUMBER: return "NUMBER";
    case TokenType::FLOAT: return "FLOAT";
    case TokenType::STRING_LITERAL: return "STRING_LITERAL";
    case TokenType::CHAR_LITERAL: return "CHAR_LITERAL";
    case TokenType::BOOLEAN_LITERAL: return "BOOLEAN_LITERAL";
    case TokenType::IDENTIFIER: return "IDENTIFIER";
    case TokenType::KEYWORD_INT: return "KEYWORD_INT";
    case TokenType::KEYWORD_STR: return "KEYWORD_STR";
    case TokenType::KEYWORD_FLOAT: return "KEYWORD_FLOAT";
    case TokenType::KEYWORD_BOOL: return "KEYWORD_BOOL";
    case TokenType::KEYWORD_IF: return "KEYWORD_IF";
    case TokenType::KEYWORD_ELSE: return "KEYWORD_ELSE";
    case TokenType::KEYWORD_WHILE: return "KEYWORD_WHILE";
    case TokenType::KEYWORD_FOR: return "KEYWORD_FOR";
    case TokenType::KEYWORD_PRINT: return "KEYWORD_PRINT";
    case TokenType::KEYWORD_INPUT: return "KEYWORD_INPUT";
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

struct Token
{
  TokenType type;
  std::string value;
  int row;
  int column;

  void print() const
  {
    std::cout << "Token(" << typeToString()
      << ", \"" << value << "\", "
      << row << column << ")" << std::endl;
  }

  std::string getPrint() const
  {
      return "Token(" + typeToString()
        + ", \"" + value + "\", "
        + std::to_string(row ) + std::to_string(column) +  ")";
  }

  std::string typeToString() const 
  {
    return tokenTypeToString(type);
  }
};

