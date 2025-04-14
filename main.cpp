#include <iostream>
#include <string>
#include <fstream>
#include <cctype>

#include "token.hpp"
#include "logger.hpp"
#include "parser.hpp"
#include "astprinter.hpp"
#include "utils.hpp"

int main()
{
  std::ifstream file("codigo.txt");

  if (!file)
  {
    LOG(LogLevel::ERROR, "No se ha podido abrir el fichero de entrada");
    return 1;
  }

  LOG(LogLevel::INFO, "INICIALIZANDO LEXER...");
  Lexer lexer(file);
  LOG(LogLevel::INFO, "PROCESANDO LEXER...");
  std::vector<Token> tokens = getAllTokens(lexer);
  LOG(LogLevel::INFO, "FINALIZANDO LEXER...");

  LOG(LogLevel::INFO, "se han generado: " + std::to_string(tokens.size()) + " tokens");
  for (const auto token: tokens) token.print();
  
  LOG(LogLevel::INFO, "Analisis lexico correctamente finalizado");

  LOG(LogLevel::INFO, "Comenzando el Parser");
  LOG(LogLevel::INFO, "Generado clase parser...");
  Parser parser(tokens);
  LOG(LogLevel::INFO, "Clase parser generada...");
  LOG(LogLevel::INFO, "MOSTRANDO LOS TOKEN GENERADOS");
  for (const auto tok: tokens)
    LOG(LogLevel::DEBUG, "TokenGenerado: " + tok.getPrint());
  LOG(LogLevel::INFO, "INICIANDO EL PARSER...");
  auto statements = parser.parse();
  LOG(LogLevel::INFO, "FINALIZANDO EL PARSER...");

  std::cout << "=== AST modo clásico ===\n";
  for (const auto& stmt : statements) {
      printStatementClassic(stmt);
  }

  std::cout << "\n=== AST modo árbol ===\n";
  for (size_t i = 0; i < statements.size(); ++i) {
      bool isLast = (i == statements.size() - 1);
      printStatementTree(statements[i], "", isLast);
  }

  LOG(LogLevel::INFO, "Finalizando el Parser");

  return 0;
}
