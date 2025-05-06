#pragma once
#include <string>

enum class SymbolCategory {
    Variable,
    Function,
    Constant,
    Parameter
};

struct Symbol {
    std::string name;
    std::string type;
    SymbolCategory category;
    std::string scope;
    std::string value;
    int line;
    int column;
};
