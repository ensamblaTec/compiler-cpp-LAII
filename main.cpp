#include <iostream>
#include <string>
#include <fstream>
#include <cctype>

#include "token.hpp"
#include "lexer.hpp"
#include "logger.hpp"

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

  while ((token = lexer.nextToken()).type != TokenType::END_OF_FILE) {
    if (token.type== TokenType::INVALID)
    {
      LOG(LogLevel::ERROR, "Error lexico en linea: " + std::to_string(token.row) 
          + ", columna: " + std::to_string(token.column) 
          + ", caracter invalido '" + token.value
      );
      continue;
    }

    token.print();
  }
  
  std::cout << "analisis lexico correctamente finalizado" << std::endl;
  return 0;
}
