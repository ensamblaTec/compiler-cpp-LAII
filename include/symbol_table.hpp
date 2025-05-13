#pragma once

#include <map>
#include <string>
#include <vector>

#include "symbol.hpp"

class SymbolTable {
public:
  SymbolTable();

  std::string getCurrentScope() const ;

  std::map<std::string, std::map<std::string, Symbol>> getScopes() const;

  void enterScope(const std::string& scopeName = "");
  void exitScope();

  bool declare(const Symbol& symbol);
  bool isDeclared(const std::string& name) const;
  bool validateVarDeclared(const std::string& name, int row, int col);
  bool updateValue(const std::string& name, const std::string& newValue, int line, int column);

  std::string getType(const std::string& name) const;
  Symbol getSymbol(const std::string& name) const;

  void printTable() const;
  void printScopeStack() const;
  void printHistory() const;

  std::vector<std::string> scopeStack;
  std::map<std::string, int> blockCounters;

  std::vector<std::string> history;

  void exportToCSV(const std::string& filepath) const;

private:
  std::map<std::string, std::map<std::string, Symbol>> scopes;
  std::string currentScope = "global";
};

