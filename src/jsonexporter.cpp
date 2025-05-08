#include "jsonexporter.hpp"

std::string categoryToString(SymbolCategory cat) {
    switch (cat) {
        case SymbolCategory::Variable: return "Variable";
        case SymbolCategory::Function: return "Function";
        case SymbolCategory::Constant: return "Constant";
        case SymbolCategory::Parameter: return "Parameter";
        default: return "Unknown";
    }
}

void to_json(json& j, const Symbol& sym) {
    j = json{
        {"name", sym.name},
        {"type", sym.type},
        {"category", categoryToString(sym.category)},
        {"scope", sym.scope},
        {"value", sym.value},
        {"line", sym.line},
        {"column", sym.column}
    };
}
