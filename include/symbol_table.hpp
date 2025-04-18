#pragma once

#include <unordered_map>
#include <optional>
#include <string>
#include <vector>

enum class VarType {
  INT,
  STR,
  BOOL,
  INVALID
};

struct SymbolInfo {
  VarType type;
};

inline std::string varTypeToString(VarType type) {
    switch (type) {
        case VarType::INT: return "entero";
        case VarType::STR: return "texto";
        case VarType::BOOL: return "bool";
        default: return "invalido";
    }
}

class SymbolTable {
public:
  SymbolTable();
  void enterScope();
  void exitScope();
  void clear();

  bool declare(const std::string& name, VarType type);

  std::optional<VarType> lookup(const std::string& name) const;

private:
  std::vector<std::unordered_map<std::string, SymbolInfo>> table;
};

