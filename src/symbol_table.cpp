#include "symbol_table.hpp"
#include "symbol.hpp"

SymbolTable::SymbolTable() {
  scopes.emplace_back();
}

void SymbolTable::enterScope() {
  scopes.emplace_back();
}

void SymbolTable::exitScope() {
  if (!scopes.empty())
    scopes.pop_back();
}

bool SymbolTable::declare(const std::string& name, const std::string& type) {
  auto& current = scopes.back();
  if (current.count(name)) return false;
  current[name] = type;
  return true;
}

bool SymbolTable::isDeclared(const std::string& name) const {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    if (it->count(name)) return true;
  }
  return false;
}

std::string SymbolTable::getType(const std::string& name) const {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) return found->second;
  }
  return "";
}

