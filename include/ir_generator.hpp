#pragma once

#include <string>
#include <vector>
#include <memory>
#include "ast.hpp"

struct IRInstruction {
  std::string op;
  std::string arg1;
  std::string arg2;
  std::string result;

  IRInstruction(const std::string& op, const std::string& arg1, const std::string& arg2, const std::string& result)
    : op(op), arg1(arg1), arg2(arg2), result(result) {}
};

class IRGenerator {
  public:
    std::vector<IRInstruction> instructions;

    std::string generateFromExpression(const std::shared_ptr<Expression>& expr);
    void generateFromStatement(const std::shared_ptr<Statement>& stmt);
    std::vector<IRInstruction> generateFromAST(const std::vector<std::shared_ptr<Statement>>& statements);
    void printIRStats(const std::vector<IRInstruction>& ir, const std::string& nombre);
  private:
    int tempCounter = 0;
    std::string newTemp();
};
