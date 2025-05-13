#include "symbol.hpp"

Symbol::Symbol(
    std::string name, 
    std::string type, 
    SymbolCategory category, 
    std::string scope, 
    std::string value, 
    int line, 
    int column
  )
  : name(name), 
  type(type), 
  category(category), 
  scope(scope), 
  value(value), 
  line(line), 
  column(column) {}
