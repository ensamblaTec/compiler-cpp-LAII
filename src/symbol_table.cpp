#include <iostream>
#include <map>

#include "symbol_table.hpp"
#include "logger.hpp"
#include "error_reporter.hpp"

SymbolTable::SymbolTable() {
  currentScope = "global";
  scopes["global"] = {};
}

void SymbolTable::enterScope(const std::string& scopeName) {
  std::string newScope = currentScope + "::" + scopeName + "_" + std::to_string(++blockCounters[scopeName]); 
  currentScope = newScope;
  scopeStack.push_back(currentScope);
  scopes[currentScope] = {};

  LOG(LogLevel::DEBUG, "Nuevo Scope creado: " + currentScope);
}

void SymbolTable::exitScope() {
  if (!scopeStack.empty()) {
    scopeStack.pop_back();
    currentScope = scopeStack.empty() ? "global" : scopeStack.back();

    LOG(LogLevel::DEBUG, "Scope restaurado a: " + currentScope);
  }
}

std::string SymbolTable::getCurrentScope() const {
  return currentScope;
}

bool SymbolTable::declare(const Symbol& symbol) {
  auto& currentSymbols = scopes[currentScope];
  if (currentSymbols.count(symbol.name)) return false;
  currentSymbols[symbol.name] = symbol;

  history.push_back("[DECLARACIÓN] var '" + symbol.name + "' tipo '" + symbol.type +
    "' en scope '" + currentScope + "' (línea " +
    std::to_string(symbol.line) + ", col " + std::to_string(symbol.column) + ")"
  );

  return true;
}

bool SymbolTable::isDeclared(const std::string& name) const {
    std::string scope = currentScope;

    while (true) {
        auto it = scopes.find(scope);
        if (it != scopes.end() && it->second.count(name)) {
            return true;
        }

        size_t pos = scope.rfind("::");
        if (pos == std::string::npos) break;
        scope = scope.substr(0, pos);
    }

    return scopes.at("global").count(name) > 0;
}

bool SymbolTable::updateValue(const std::string& name, const std::string& newValue, int line, int column) {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto& table = it->second;
    if (table.count(name)) {
      table[name].value = newValue;
      table[name].line = line;
      table[name].column = column;
      history.push_back("[ASIGNACIÓN] var '" + name + "' nuevo valor '" + newValue +
        "' en scope '" + it->first + "' (línea " +
        std::to_string(line) + ", col " + std::to_string(column) + ")"
      );
      return true;
    }
  }
  return false;
}

std::string SymbolTable::getType(const std::string& name) const {
    std::string scope = currentScope;

    while (true) {
        auto it = scopes.find(scope);
        if (it != scopes.end() && it->second.count(name)) {
            return it->second.at(name).type;
        }

        size_t pos = scope.rfind("::");
        if (pos == std::string::npos) break;
        scope = scope.substr(0, pos);
    }

    return scopes.at("global").at(name).type;
}

Symbol SymbolTable::getSymbol(const std::string& name) const {
  for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
    auto found = it->second.find(name);
    if (found != it->second.end()) {
      return found->second;
    }
  }

  return Symbol{"", "error", SymbolCategory::Variable, "", "", -1, -1};
}

void SymbolTable::printTable() const {
    std::cout << "\n===== Tabla de Símbolos =====\n";
    for (const auto& [scope, symbols] : scopes) {
        std::cout << "Scope [" << scope << "]\n";
        for (const auto& [name, sym] : symbols) {
            std::cout << "  - name: " << sym.name
                      << ", type: " << sym.type
                      << ", category: " << static_cast<int>(sym.category)
                      << ", scope: " << sym.scope
                      << ", value: " << sym.value
                      << ", line: " << sym.line
                      << ", column: " << sym.column << "\n";
        }
    }
    std::cout << "==============================\n";
}

std::map<std::string, std::map<std::string, Symbol>> SymbolTable::getScopes() const {
  return scopes;
}

void SymbolTable::printScopeStack() const {
    std::cout << "\n--- Pila de Scopes (scopeStack) ---\n";
    for (const auto& scope : scopeStack) {
        std::cout << "  - " << scope << "\n";
    }
    std::cout << "-----------------------------------\n";
}

void SymbolTable::printHistory() const {
  std::cout << "\n===== Historial de Símbolos =====\n";
  for (const auto& entry : history) {
    std::cout << "  - " << entry << "\n";
  }
  std::cout << "==================================\n";
}

bool SymbolTable::validateVarDeclared(const std::string& name, int row, int col) {
    if (!isDeclared(name)) {
        std::string msg = "La variable '" + name + "' no ha sido declarada en ningún ámbito visible.";
        LOG(LogLevel::ERROR, "[validateVarDeclared] " + msg);
        ErrorReporter::getInstance().report(msg, row, col);
        return false;
    }
    return true;
}
