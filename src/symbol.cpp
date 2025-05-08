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
  column(column) {
    if (this->value.empty()) {
      if (this->type == "entero") {
          this->value = "0";
      } else if (this->type == "texto") {
          this->value = "\"\"";  // o "" si no usas comillas explícitas
      } else if (this->type == "bool") {
          this->value = "falso";
      } else {
          this->value = "null";  // opción por defecto si no se reconoce el tipo
      }
    }
  }
