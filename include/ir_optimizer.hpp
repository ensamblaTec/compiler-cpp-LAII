#pragma once

#include "ir_generator.hpp"
#include <unordered_map>
#include <string>

class IROptimizer {
  public:
    void execute(const std::vector<IRInstruction>& instructions);

    bool isNumeric(const std::string& s);
    bool isStringLiteral(const std::string& s);

  private:
    std::unordered_map<std::string, std::string> variables;
    size_t instructionPointer = 0;

    std::string getValue(const std::string& operand);
    bool isTruthy(const std::string& val);
};
