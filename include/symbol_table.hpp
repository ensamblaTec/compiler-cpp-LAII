#pragma once

#include <unordered_map>
#include <string>
#include <vector>

class SymbolTable {
public:
  void enterScope();   // Nuevo bloque (nueva tabla local)
  void exitScope();    // Salir del bloque

  bool declare(const std::string& name);  // Devuelve true si fue exitoso, false si ya existía
  bool isDeclared(const std::string& name) const;

private:
  std::vector<std::unordered_map<std::string, bool>> scopes;
};

