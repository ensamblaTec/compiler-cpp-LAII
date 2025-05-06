#include <iostream>
#include <string>
#include <fstream>
#include <cctype>

#include "token.hpp"
#include "logger.hpp"
#include "parser.hpp"
#include "astprinter.hpp"
#include "utils.hpp"
#include "jsonexporter.hpp"

int main(int argc, char* argv[])
{
  if (argc < 2) {
    LOG(LogLevel::ERROR, "Se requiere el nombre del fichero como argumento");
    return 1;
  }

  const char* folderName = "output";
  createFolder(folderName);

  std::string fileName = argv[1];

  std::ifstream file(fileName);

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

  saveFile("tabla_simbolos", symbolTableToString(tokens));

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

  // json exporter
  Parser parser1(tokens);
  auto ast = parser1.parse();

  json programJson = json::array();
  for (auto& stmt : ast) {
      programJson.push_back(statementToJson(stmt));
  }

  std::ofstream out("output/ast.json");
  out << programJson.dump(4);
  out.close();

  std::cout << "✅ AST exportado a output/ast.json" << std::endl;

  return 0;
}
