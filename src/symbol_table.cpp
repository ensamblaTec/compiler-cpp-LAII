#include "symbol_table.hpp"

void SymbolTable::enterScope() {
  scopes.emplace_back();
}

void SymbolTable::exitScope() {
  if (!scopes.empty()) {
    scopes.pop_back();
  }
}

bool SymbolTable::declare(const std::string& name) {
  if (scopes.empty()) enterScope();
  auto& current = scopes.back();
  if (current.count(name)) return false;
  current[name] = true;
  return true;
}

bool SymbolTable::isDeclared(const std::string& name) const {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    if (it->count(name)) return true;
  }
  return false;
}

