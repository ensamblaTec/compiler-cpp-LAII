#include "lexer.hpp"
#include "logger.hpp"

#include <iostream>
#include <cctype>
#include <unordered_set>
#include <unordered_map>

static const std::unordered_set<std::string> keywords = {
    "entero", "decimal", "booleano", "texto", "caracter", "metodo",
    "si", "no", "mientras", "para", "retornar",
    "romper", "continuar", "verdadero", "falso"
};


static const std::unordered_map<std::string, TokenType> operatorMap = {
    {"+", TokenType::PLUS}, {"-", TokenType::MINUS}, {"*", TokenType::MUL}, {"/", TokenType::DIV}, {"%", TokenType::MOD},
    {"=", TokenType::ASSIGN}, {"==", TokenType::EQ}, {"!=", TokenType::NEQ},
    {">", TokenType::GT}, {"<", TokenType::LT}, {">=", TokenType::GTE}, {"<=", TokenType::LTE},
    {"+=", TokenType::PLUS_ASSIGN}, {"-=", TokenType::MINUS_ASSIGN}, {"*=", TokenType::MUL_ASSIGN},
    {"/=", TokenType::DIV_ASSIGN}, {"%=", TokenType::MOD_ASSIGN},
    {"&&", TokenType::AND}, {"||", TokenType::OR}, {"!", TokenType::NOT}
};

static const std::unordered_map<char, TokenType> punctuationMap = {
    {';', TokenType::SEMICOLON}, {',', TokenType::COMMA}, {'.', TokenType::DOT}, {':', TokenType::COLON},
    {'(', TokenType::LPAREN}, {')', TokenType::RPAREN},
    {'{', TokenType::LBRACE}, {'}', TokenType::RBRACE},
    {'[', TokenType::LBRACKET}, {']', TokenType::RBRACKET}
};

Lexer::Lexer(std::istream& input_stream) : input(input_stream)
{
  LOG(LogLevel::INFO, "Iniciando el lexer...");
  LOG(LogLevel::INFO, "Se ha generado el fichero de logs");
  LOG(LogLevel::INFO, "Preparando el fichero de entrada para su procesamiento");

  readNextLine();

  LOG(LogLevel::INFO, "Linea cargada exitosamente...");
}

void Lexer::readNextLine()
{
  if (std::getline(input, currentLine))
  {
    currentLine += ' ';
    pos = 0;
    row++;
    column = 1;
    std::string msgCurrentLine = "Linea: " + std::to_string(row) + " leido: \"" + currentLine + "\"";
    LOG(LogLevel::INFO, msgCurrentLine);  
  } else
  {
    currentLine = "";
    LOG(LogLevel::INFO, "Fin del fichero...");
  }
}

Token Lexer::nextToken()
{
  while (true)
  {
    if (pos >= currentLine.size())
    {
      readNextLine();
      if (currentLine.empty())
      {
        return { TokenType::END_OF_FILE, "", row, column };
      }
    }

    char currentCharacter = currentLine[pos];

    if (std::isspace(currentCharacter))
    {
      pos++;
      column++;
      continue;
    }

    int startColumn = column;

    if (currentCharacter == '/' && pos + 1 < currentLine.size() && currentLine[pos+1] == '/')
    {
      readNextLine();
      continue;
    }

    if (currentCharacter == '\"')
    {
      std::string buffer;
      pos++;
      column++;

      while (pos < currentLine.size() && currentLine[pos] != '\"')
      {
        buffer += currentLine[pos++];
        column++;
      }

      pos++;
      column++;

      return { TokenType::STRING_LITERAL, buffer, row, startColumn };
    }

    if (std::isalpha(currentCharacter) || currentCharacter == '_')
    {
      std::string buffer;
      while (pos < currentLine.size() && std::isalnum(currentLine[pos]) || currentLine[pos] == '_')
      {
        buffer += currentLine[pos++];
        column++;
      }

      TokenType type = keywords.count(buffer) ? TokenType::KEYWORD : TokenType::IDENTIFIER;
      return { type, buffer, row, startColumn };
    }

    if (std::isdigit(currentCharacter))
    {
      std::string buffer;
      while (pos < currentLine.size() && std::isdigit(currentLine[pos]))
      {
        buffer += currentLine[pos++];
        column++;
      }

      return { TokenType::NUMBER, buffer, row, startColumn };
    }

    if (pos + 1 < currentLine.size())
    {
      std::string operatorTwoChar = currentLine.substr(pos, 2);
      if (operatorMap.count(operatorTwoChar))
      {
        pos += 2;
        column += 2;
        return { operatorMap.at(operatorTwoChar), operatorTwoChar, row, startColumn };
      }
    }

    std::string oneCharacter(1, currentCharacter);
    if (operatorMap.count(oneCharacter))
    {
      pos++;
      column++;
      return { operatorMap.at(oneCharacter), oneCharacter, row, startColumn };
    }

    if (punctuationMap.count(currentCharacter))
    {
      pos++;
      column++;
      return { punctuationMap.at(currentCharacter), std::string(1, currentCharacter), row, startColumn };
    }

    pos++;
    column++;
    return { TokenType::INVALID, std::string(1, currentCharacter), row, startColumn };
  }
}
