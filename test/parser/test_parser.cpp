#include "parser.hpp"
#include "utils.hpp"
#include "astprinter.hpp"

#include <iostream>
#include <fstream>

void testParser(const std::string& path) {
  std::ifstream file(path);
  Lexer lexer(file);
  std::vector<Token> tokens = getAllTokens(lexer);

  Parser parser(tokens);
  auto statements = parser.parse();

  for (auto& stmt: statements)
    printStatementClassic(stmt);
}

int main() {
  testParser("test/parser/valid_all_input_token.txt");
  return 0;
}
