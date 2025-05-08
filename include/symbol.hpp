#pragma once
#include <string>

enum class SymbolCategory {
    Variable,
    Function,
    Constant,
    Parameter
};

struct Symbol {
  Symbol() = default;

  Symbol(std::string name, std::string type, SymbolCategory category, std::string scope, std::string value, int line, int column);

  std::string name;
  std::string type;
  SymbolCategory category;
  std::string scope;
  std::string value;
  int line;
  int column;

  std::string categoryToString() const {
    switch (category) {
      case SymbolCategory::Variable: return "Variable";
      case SymbolCategory::Function: return "Function";
      case SymbolCategory::Constant: return "Constant";
      case SymbolCategory::Parameter: return "Parameter";
      default: return "INVALID";
    }
  }
};
