#include "symbol_table.hpp"

SymbolTable::SymbolTable() {
  enterScope();
}

void SymbolTable::enterScope() {
  table.emplace_back();
}

void SymbolTable::exitScope() {
  if (!table.empty()) {
    table.pop_back();
  }
}

bool SymbolTable::declare(const std::string& name, VarType type) {
  if (table.empty()) enterScope();
  auto& current = table.back();
  if (current.count(name)) return false;
  current[name] = SymbolInfo{ type };
  return true;
}

std::optional<VarType> SymbolTable::lookup(const std::string& name) const {
  for (auto it = table.rbegin(); it != table.rend(); ++it) {
    auto found = it->find(name);
    if (found != it->end()) return found->second.type;
  }
  return std::nullopt;
}

void SymbolTable::clear() {
  table.clear();
  enterScope();
}
