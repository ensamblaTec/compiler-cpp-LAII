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
    if (s == "true" || s == "false") return false;
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
  result = constantFolding(result);
  result = eliminateDeadCode(result);
  result = commonSubexpressionElimination(result);
  result = loopInvariantCodeMotion(result);
  result = constantPropagation(result);
  result = peepholeOptimize(result);
  result = staticSingleAssignment(result);

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

std::vector<IRInstruction> IROptimizer::constantFolding(const std::vector<IRInstruction>& instructions) {
    std::vector<IRInstruction> result;

    for (const auto& instr : instructions) {
        const std::string& op = instr.op;

        if ((op == "+" || op == "-" || op == "*" || op == "/" || op == "%") &&
            isNumeric(instr.arg1) && isNumeric(instr.arg2)) {

            int left = std::stoi(instr.arg1);
            int right = std::stoi(instr.arg2);
            int folded = 0;

            if (op == "+") folded = left + right;
            else if (op == "-") folded = left - right;
            else if (op == "*") folded = left * right;
            else if (op == "/") folded = (right != 0 ? left / right : 0);
            else if (op == "%") folded = (right != 0 ? left % right : 0);

            result.push_back({"ASSIGN", std::to_string(folded), "", instr.result});
        }

        else if ((op == "<" || op == ">" || op == "<=" || op == ">=" || op == "==" || op == "!=") &&
                 isNumeric(instr.arg1) && isNumeric(instr.arg2)) {

            int left = std::stoi(instr.arg1);
            int right = std::stoi(instr.arg2);
            bool cond = false;

            if (op == "<") cond = left < right;
            else if (op == ">") cond = left > right;
            else if (op == "<=") cond = left <= right;
            else if (op == ">=") cond = left >= right;
            else if (op == "==") cond = left == right;
            else if (op == "!=") cond = left != right;

            result.push_back({"ASSIGN", cond ? "1" : "0", "", instr.result});
        }

        else {
            result.push_back(instr);
        }
    }

    return result;
}

std::vector<IRInstruction> IROptimizer::commonSubexpressionElimination(const std::vector<IRInstruction>& ir) {
    std::vector<IRInstruction> optimized;
    std::map<std::string, std::string> exprMap;

    for (const auto& instr : ir) {
        if (instr.op == "+" || instr.op == "-" || instr.op == "*" || instr.op == "/" || instr.op == "%") {
            std::string key = instr.op + ":" + instr.arg1 + "," + instr.arg2;
            if (exprMap.count(key)) {
                optimized.push_back({"ASSIGN", exprMap[key], "", instr.result});
            } else {
                optimized.push_back(instr);
                exprMap[key] = instr.result;
            }
        } else {
            optimized.push_back(instr);
        }
    }

    return optimized;
}

std::vector<IRInstruction> IROptimizer::loopInvariantCodeMotion(const std::vector<IRInstruction>& ir) {
    std::vector<IRInstruction> preLoop;
    std::vector<IRInstruction> loopBody;
    std::vector<IRInstruction> postLoop;

    bool inLoop = false;
    std::string loopStartLabel;

    for (const auto& instr : ir) {
        if (instr.op == "LABEL" && instr.arg1.find("while") != std::string::npos) {
            inLoop = true;
            loopStartLabel = instr.arg1;
            preLoop.push_back(instr);
            continue;
        }

        if (inLoop && instr.op == "LABEL") {
            inLoop = false;
            postLoop.push_back(instr);
            continue;
        }

        if (inLoop) {
            loopBody.push_back(instr);
        } else if (!inLoop) {
            if (loopStartLabel.empty()) preLoop.push_back(instr);
            else postLoop.push_back(instr);
        }
    }

    std::set<std::string> assigned;
    for (const auto& instr : loopBody) {
        if (instr.op == "ASSIGN" || instr.op == "+" || instr.op == "*" || instr.op == "-") {
            assigned.insert(instr.result);
        }
    }

    std::vector<IRInstruction> invariant, newBody;
    for (const auto& instr : loopBody) {
        if ((instr.op == "+" || instr.op == "-" || instr.op == "*" || instr.op == "/" || instr.op == "%") &&
            assigned.find(instr.arg1) == assigned.end() &&
            assigned.find(instr.arg2) == assigned.end()) {
            invariant.push_back(instr);
        } else {
            newBody.push_back(instr);
        }
    }

    std::vector<IRInstruction> result = preLoop;
    result.insert(result.end(), invariant.begin(), invariant.end());
    result.insert(result.end(), newBody.begin(), newBody.end());
    result.insert(result.end(), postLoop.begin(), postLoop.end());
    return result;
}

std::vector<IRInstruction> IROptimizer::constantPropagation(const std::vector<IRInstruction>& instructions) {
    std::unordered_map<std::string, std::string> constants;
    std::vector<IRInstruction> result;

    for (const auto& instr : instructions) {
        IRInstruction newInstr = instr;

        if (instr.op == "ASSIGN" && isNumeric(instr.arg1)) {
            constants[instr.result] = instr.arg1;
        } else if (instr.op == "ASSIGN" && constants.count(instr.arg1)) {
            newInstr.arg1 = constants[instr.arg1];
            constants[instr.result] = newInstr.arg1;
        } else if (instr.op == "+" || instr.op == "-" || instr.op == "*" || instr.op == "/" || instr.op == "%") {
            if (constants.count(instr.arg1)) newInstr.arg1 = constants[instr.arg1];
            if (constants.count(instr.arg2)) newInstr.arg2 = constants[instr.arg2];
            constants.erase(instr.result);
        } else {
            constants.erase(instr.result);
        }

        result.push_back(newInstr);
    }

    return result;
}

std::vector<IRInstruction> IROptimizer::peepholeOptimize(const std::vector<IRInstruction>& input) {
    std::vector<IRInstruction> optimized;

    for (size_t i = 0; i < input.size(); ++i) {
        const auto& instr = input[i];

        if (instr.op == "ASSIGN" && i + 1 < input.size()) {
            const auto& next = input[i + 1];
            if (next.op == "ASSIGN" && next.arg1 == instr.result) {
                optimized.push_back({ "ASSIGN", instr.arg1, "", next.result });
                ++i;
                continue;
            }
        }

        if ((instr.op == "+" || instr.op == "-" || instr.op == "*") && instr.result != "") {
            if (instr.arg2 == "0" && instr.op == "+") {
                optimized.push_back({ "ASSIGN", instr.arg1, "", instr.result });
                continue;
            }
            if (instr.arg2 == "0" && instr.op == "-") {
                optimized.push_back({ "ASSIGN", instr.arg1, "", instr.result });
                continue;
            }
            if (instr.arg2 == "1" && instr.op == "*") {
                optimized.push_back({ "ASSIGN", instr.arg1, "", instr.result });
                continue;
            }
            if (instr.arg2 == "0" && instr.op == "*") {
                optimized.push_back({ "ASSIGN", "0", "", instr.result });
                continue;
            }
        }

        optimized.push_back(instr);
    }

    return optimized;
}

std::vector<IRInstruction> IROptimizer::staticSingleAssignment(const std::vector<IRInstruction>& ir) {
    std::unordered_map<std::string, int> version;
    std::unordered_map<std::string, std::string> currentName;
    std::vector<IRInstruction> result;

    auto rename = [&](const std::string& var) -> std::string {
        if (isNumeric(var) || isStringLiteral(var) || var.empty())
            return var;
        return currentName.count(var) ? currentName[var] : var;
    };

    for (const auto& instr : ir) {
        IRInstruction newInstr = instr;

        newInstr.arg1 = rename(instr.arg1);
        newInstr.arg2 = rename(instr.arg2);

        if (!instr.result.empty()) {
            int v = ++version[instr.result];
            std::string ssaName = instr.result + std::to_string(v);
            currentName[instr.result] = ssaName;
            newInstr.result = ssaName;
        }

        result.push_back(newInstr);
    }

    return result;
}

