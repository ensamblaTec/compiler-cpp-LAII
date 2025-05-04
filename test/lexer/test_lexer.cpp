#include "lexer.hpp"
#include "utils.hpp"

#include <iostream>
#include <fstream>

void testLexer(const std::string& path) {
  std::ifstream file(path);
  Lexer lexer(file);
  std::vector<Token> tokens = getAllTokens(lexer);

  for (const auto token: tokens) std::cout << "token: " << token.value << " -> " << token.typeToString() << std::endl;
}

int main() {
  testLexer("./test/lexer/valid_general.txt");
  return 0;
}
