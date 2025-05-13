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
#include "ir_generator.hpp"
#include "ir_optimizer.hpp"
#include "asm_generator.hpp"

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

  // LOG(LogLevel::INFO, "se han generado: " + std::to_string(tokens.size()) + " tokens");
  // for (const auto token: tokens) token.print();
  
  LOG(LogLevel::INFO, "INICIANDO PARSER...");
  Parser parser(tokens);
  // LOG(LogLevel::INFO, "MOSTRANDO LOS TOKEN GENERADOS");
  // for (const auto tok: tokens)
  //   LOG(LogLevel::DEBUG, "TokenGenerado: " + tok.getPrint());
  auto statements = parser.parse();
  LOG(LogLevel::INFO, "FINALIZANDO EL PARSER...");

  exportAstToJsonFile(statements, "output/ast.json");
  std::cout << "[INFO] AST exportado a ../output/ast.json\n";

  IRGenerator generator;
  std::vector<IRInstruction> ir_code = generator.generateFromAST(statements);
  std::string bufferIR;

  std::cout << "=== Código Intermedio (TAC) ===\n";
  for (const auto& instr : ir_code) {
      std::cout << instr.op << " " << instr.arg1 << ", " << instr.arg2 << " -> " << instr.result << std::endl;
      bufferIR = bufferIR + instr.op + "," + instr.arg1 + "," + instr.arg2 + "," + instr.result + "\n";
  }
  saveFile("ir", bufferIR);

  std::cout << "=== Optimizacion Código Intermedio (TAC) ===\n";
  IROptimizer optimizer;
  // optimizer.execute(ir_code);

  std::vector<IRInstruction> ir_code_optimized = optimizer.optimize(ir_code);
  for (const auto& instr : ir_code_optimized) {
    std::cout << instr.op << " " << instr.arg1 << ", " << instr.arg2 << " -> " << instr.result << std::endl;
    bufferIR = bufferIR + instr.op + "," + instr.arg1 + "," + instr.arg2 + "," + instr.result + "\n";
  }

  std::cout << "=== AST modo clásico ===\n";
  for (const auto& stmt : statements) {
    printStatementClassic(stmt);
  }

  std::cout << "\n=== AST modo árbol ===\n";
  for (size_t i = 0; i < statements.size(); ++i) {
    bool isLast = (i == statements.size() - 1);
    printStatementTree(statements[i], "", isLast);
  }

  std::cout << "\n=== SALIDA ===\n";
  AsmGenerator asmGenerator;
  std::string asmCode = asmGenerator.generate(ir_code_optimized);

  std::cout << "\n=== SALIDA ASM RISC-V ===\n" << asmCode << std::endl; 

  return 0;
}

