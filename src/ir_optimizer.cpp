#include "ir_optimizer.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include <regex>
#include <map>
#include <vector>
#include <set>

bool IROptimizer::isNumeric(const std::string& s) {
    if (s.empty()) return false;
    if (s[0] == '-' && s.size() > 1) return std::all_of(s.begin() + 1, s.end(), ::isdigit);
    return std::all_of(s.begin(), s.end(), ::isdigit);
}

bool IROptimizer::isStringLiteral(const std::string& s) {
    return s.size() >= 2 && s.front() == '"' && s.back() == '"';
}

void IROptimizer::execute(const std::vector<IRInstruction>& instructions) {
    std::map<std::string, std::string> variables;
    std::map<std::string, size_t> labels;

    auto isNumeric = [](const std::string& s) {
        if (s.empty()) return false;
        if (s[0] == '-' && s.size() > 1) return std::all_of(s.begin() + 1, s.end(), ::isdigit);
        return std::all_of(s.begin(), s.end(), ::isdigit);
    };

    auto isStringLiteral = [](const std::string& s) {
        return s.size() >= 2 && s.front() == '"' && s.back() == '"';
    };

    auto getValue = [&](const std::string& token) {
        if (isStringLiteral(token)) return token;
        if (isNumeric(token)) return token;
        if (variables.count(token)) return variables[token];
        return std::string("0");
    };

    for (size_t i = 0; i < instructions.size(); ++i) {
        if (instructions[i].op == "LABEL") {
            labels[instructions[i].arg1] = i;
        }
    }

    for (size_t ip = 0; ip < instructions.size(); ++ip) {
        const auto& instr = instructions[ip];
        const std::string& op = instr.op;

        if (op == "ASSIGN") {
            variables[instr.result] = getValue(instr.arg1);
        }
        else if (op == "PRINT") {
            std::string val = getValue(instr.arg1);
            if (isStringLiteral(val)) val = val.substr(1, val.length() - 2);
            std::cout << val << std::endl;
        }
        else if (op == "INPUT") {
            std::cout << "> ";
            std::string input;
            std::getline(std::cin, input);
            variables[instr.arg1] = input;
        }
        else if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
            int left = std::stoi(getValue(instr.arg1));
            int right = std::stoi(getValue(instr.arg2));
            int result = 0;

            if (op == "+") result = left + right;
            else if (op == "-") result = left - right;
            else if (op == "*") result = left * right;
            else if (op == "/") result = (right != 0 ? left / right : 0);
            else if (op == "%") result = (right != 0 ? left % right : 0);

            variables[instr.result] = std::to_string(result);
        }
        else if (op == ">" || op == "<" || op == "==" || op == ">=" || op == "<=" || op == "!=") {
            int left = std::stoi(getValue(instr.arg1));
            int right = std::stoi(getValue(instr.arg2));
            bool cond = false;

            if (op == ">") cond = left > right;
            else if (op == "<") cond = left < right;
            else if (op == "==") cond = left == right;
            else if (op == ">=") cond = left >= right;
            else if (op == "<=") cond = left <= right;
            else if (op == "!=") cond = left != right;

            variables[instr.result] = cond ? "1" : "0";
        }
        else if (op == "IF_FALSE_GOTO") {
            int cond = std::stoi(getValue(instr.arg1));
            if (cond == 0) {
                ip = labels[instr.result];
                continue;
            }
        }
        else if (op == "GOTO") {
            ip = labels[instr.result];
            continue;
        }
        else if (op == "LABEL") {
            continue;
        }
    }
}

std::string IROptimizer::getValue(const std::string& operand) {
    if (variables.count(operand)) return variables[operand];
    return operand;
}

bool IROptimizer::isTruthy(const std::string& val) {
    return val != "0" && val != "falso";
}

std::vector<IRInstruction> IROptimizer::eliminateDeadCode(const std::vector<IRInstruction>& input) {
  std::set<std::string> usedVars;

  for (const auto& instr: input) {
    if (!instr.arg1.empty()) usedVars.insert(instr.arg1);
    if (!instr.arg2.empty()) usedVars.insert(instr.arg2);
    if (instr.op == "PRINT" || instr.op == "INPUT" || instr.op == "GOTO" || instr.op == "IF_FALSE_GOTO") {
      usedVars.insert(instr.result);
    }
  }

  std::vector<IRInstruction> optimized;
  for (const auto& instr: input) {
    if (instr.op == "ASSIGN" && !usedVars.count(instr.result)) {
      continue;
    }
    optimized.push_back(instr);
  }

  return optimized;
}

std::vector<IRInstruction> IROptimizer::optimize(const std::vector<IRInstruction>& original) {
  std::vector<IRInstruction> result = original;

  result = eliminateDeadCode(result);
  result = copyPropagation(result);

  return result;
}

std::vector<IRInstruction> IROptimizer::copyPropagation(const std::vector<IRInstruction>& input) {
  std::unordered_map<std::string, std::string> copyTable;
  std::vector<IRInstruction> optimized;

  for (const auto& instr: input) {
    IRInstruction current = instr;

    if (current.op == "ASSIGN" && !current.arg1.empty() && current.arg2.empty()) {
      if (!isStringLiteral(current.arg1) && !isNumeric(current.arg1)) {
        copyTable[current.result] = copyTable.count(current.arg1) ? copyTable[current.arg1] : current.arg1;
        optimized.push_back(current);
        continue;
      }
    }

    if (copyTable.count(current.arg1)) {
      current.arg1 = copyTable[current.arg1];
    }

    if (copyTable.count(current.arg2)) {
      current.arg2 = copyTable[current.arg2];
    }

    optimized.push_back(current);
  }

  return optimized;
}

void printIRStats(const std::vector<IRInstruction)
