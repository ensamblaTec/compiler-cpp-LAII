#pragma once

#include <string>

enum class TokenType
{
  NUMBER,
  FLOAT,
  STRING_LITERAL,
  CHAR_LITERAL,
  BOOLEAN_LITERAL,
  IDENTIFIER,
  KEYWORD,
  PLUS, MINUS, MUL, DIV, MOD,
  ASSIGN,
  EQ, NEQ, GT, LT, GTE, LTE,
  PLUS_ASSIGN, MINUS_ASSIGN, MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN,
  AND, OR, NOT,
  SEMICOLON, COMMA, DOT, COLON,
  LPAREN, RPAREN,
  LBRACE, RBRACE,
  LBRACKET, RBRACKET,
  END_OF_FILE,
  INVALID
};

struct Token
{
  TokenType type;
  std::string value;
  int column;
  int row;

  void print() const;

  std::string tokenTypeToString(TokenType type) const;
};
