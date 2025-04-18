#pragma once

#include <unordered_map>
#include <string>
#include <vector>

class SymbolTable {
public:
  SymbolTable();
  void enterScope();
  void exitScope();

  bool declare(const std::string& name, const std::string& type);
  bool isDeclared(const std::string& name) const;

  std::string getType(const std::string& name) const;

private:
  std::vector<std::unordered_map<std::string, std::string>> scopes;
};

