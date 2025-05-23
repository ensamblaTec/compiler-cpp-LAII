#pragma once

#include "ir_generator.hpp"
#include <unordered_map>
#include <string>

class IROptimizer {
  public:
    void execute(const std::vector<IRInstruction>& instructions);

    std::vector<IRInstruction> optimize(const std::vector<IRInstruction>& original);

    bool isNumeric(const std::string& s);
    bool isStringLiteral(const std::string& s);

  private:
    std::unordered_map<std::string, std::string> variables;
    size_t instructionPointer = 0;

    std::string getValue(const std::string& operand);
    bool isTruthy(const std::string& val);
    std::vector<IRInstruction> eliminateDeadCode(const std::vector<IRInstruction>& input);
    std::vector<IRInstruction> copyPropagation(const std::vector<IRInstruction>& input);
    std::vector<IRInstruction> constantFolding(const std::vector<IRInstruction>& instructions);
    std::vector<IRInstruction> commonSubexpressionElimination(const std::vector<IRInstruction>& ir);
    std::vector<IRInstruction> loopInvariantCodeMotion(const std::vector<IRInstruction>& ir);
    std::vector<IRInstruction> constantPropagation(const std::vector<IRInstruction>& instructions);
    std::vector<IRInstruction> peepholeOptimize(const std::vector<IRInstruction>& input);
    std::vector<IRInstruction> staticSingleAssignment(const std::vector<IRInstruction>& ir);
};
