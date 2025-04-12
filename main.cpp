#include <iostream>
#include <string>
#include <fstream>
#include <cctype>

#include "token.hpp"
#include "lexer.hpp"
#include "logger.hpp"
#include "parser.hpp"
#include "astprinter.hpp"

int main()
{
  std::ifstream file("codigo.txt");

  if (!file)
  {
    LOG(LogLevel::ERROR, "No se ha podido abrir el fichero de entrada");
    return 1;
  }

  Lexer lexer(file);
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
    tokens.emplace_back(token);
    token.print();
  }

  LOG(LogLevel::INFO, "se han generado: " + std::to_string(tokens.size()) + " tokens");
  for (const auto v: tokens) std::cout << "valores : " << v.value << std::endl;
  
  LOG(LogLevel::INFO, "Analisis lexico correctamente finalizado");

  LOG(LogLevel::INFO, "Comenzando el Parser");
  LOG(LogLevel::INFO, "Generado clase parser...");
  Parser parser(tokens);
  LOG(LogLevel::INFO, "Clase parser genereada...");
  LOG(LogLevel::INFO, "Inciando el parse...");
  auto statements = parser.parse();
  LOG(LogLevel::INFO, "Finalizando el parse...");

  for (const auto& stmt: statements) {
    printStatement(stmt);
  }

  LOG(LogLevel::INFO, "Finalizando el Parser");

  return 0;
}
