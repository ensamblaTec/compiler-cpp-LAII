#include <iostream>
#include <fstream>

#include "utils.hpp"
#include "logger.hpp"

#if defined(_WIN32)
  #include <direct.h>
  #define MKDIR(path) _mkdir(path)
#else
  #include <sys/stat.h>
  #define MKDIR(path) mkdir(path, 0777)
#endif

std::vector<Token> getAllTokens(Lexer& lexer)
{
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
    tokens.push_back(token);
  }

  tokens.push_back(token);

  return tokens;
}

void tokensToCSV(std::vector<Token> tokens) {
  if (tokens.size() == 0) {
    return;
  }

  std::string buffer = "";

  for (auto token: tokens)
    buffer += token.getPrintToken() + ",\n";

  saveFile("tabla_tokens", buffer);
}

bool saveFile(std::string fileName, std::string content) {
  std::ofstream file("output/" + fileName + ".txt");

  if (file.is_open()) {
    file << content;
    file.close();
    LOG(LogLevel::INFO, "Se ha almacenado el fichero correctamente");
  } else {
    LOG(LogLevel::ERROR, "No se ha podido abrir el fichero");
  }
  return true;
}

void createFolder(const char* folderName) {
  if (MKDIR(folderName) == 0) {
    std::cout << "directorio generado correctamente";
  }
}

