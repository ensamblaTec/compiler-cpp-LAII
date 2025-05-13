#include "asm_generator.hpp"

#include <sstream>
#include <unordered_map>
#include <cctype>
#include <algorithm>
#include <iostream>

bool AsmGenerator::isNumber(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

std::string AsmGenerator::getRegister(const std::string& var) {
    if (varRegisterMap.count(var)) return varRegisterMap[var];
    std::string reg = "t" + std::to_string(tempRegCounter++);  // t0–t6 disponibles
    varRegisterMap[var] = reg;
    return reg;
}

std::string AsmGenerator::generate(const std::vector<IRInstruction>& instructions) {
    std::ostringstream out;

    out << ".data\n";
    out << "newline: .asciiz \"\\n\"\n\n";
    out << ".text\n";
    out << ".globl main\n";
    out << "main:\n";

    for (const auto& inst : instructions) {
        std::string asmLine = generateInstruction(inst);
        if (!asmLine.empty())
            out << asmLine << "\n";
    }

    out << "\nli a7, 10     # syscall: exit\n";
    out << "ecall\n";

    return out.str();
}

std::string AsmGenerator::generateInstruction(const IRInstruction& instruction) {
    const std::string& op = instruction.op;
    const std::string& a1 = instruction.arg1;
    const std::string& a2 = instruction.arg2;
    const std::string& res = instruction.result;
    
    if (op == "LABEL") {
        const std::string& label = a1;
        if (label.rfind("L", 0) == 0 || label.rfind("t", 0) == 0 || label.rfind("etq_", 0) == 0)
            return label + ":";
        else
            return "# ⚠ etiqueta ignorada: " + label;
    }


    if (op == "GOTO") {
        return "j " + res;
    }

    if (op == "IF_FALSE_GOTO") {
        std::string condReg = getRegister(a1);
        return "beqz " + condReg + ", " + res;
    }

    if (op == "ASSIGN") {
        std::string dst = getRegister(res);
        if (isNumber(a1)) {
            return "li " + dst + ", " + a1;
        } else {
            return "mv " + dst + ", " + getRegister(a1);
        }
    }

    if (op == "+") {
        return "add " + getRegister(res) + ", " + getRegister(a1) + ", " + getRegister(a2);
    }
    if (op == "-") {
        return "sub " + getRegister(res) + ", " + getRegister(a1) + ", " + getRegister(a2);
    }
    if (op == "*") {
        return "mul " + getRegister(res) + ", " + getRegister(a1) + ", " + getRegister(a2);
    }
    if (op == "/") {
        return "div " + getRegister(res) + ", " + getRegister(a1) + ", " + getRegister(a2);
    }
    if (op == "%") {
        return "rem " + getRegister(res) + ", " + getRegister(a1) + ", " + getRegister(a2);
    }

    if (op == "==") {
        return "beq " + getRegister(a1) + ", " + getRegister(a2) + ", " + res;
    }
    if (op == "!=") {
        return "bne " + getRegister(a1) + ", " + getRegister(a2) + ", " + res;
    }
    if (op == "<") {
        return "blt " + getRegister(a1) + ", " + getRegister(a2) + ", " + res;
    }
    if (op == "<=") {
        return "ble " + getRegister(a1) + ", " + getRegister(a2) + ", " + res;
    }
    if (op == ">") {
        return "bgt " + getRegister(a1) + ", " + getRegister(a2) + ", " + res;
    }
    if (op == ">=") {
        return "bge " + getRegister(a1) + ", " + getRegister(a2) + ", " + res;
    }

    if (op == "INPUT") {
        std::string dst = getRegister(a1);
        return "li a7, 5\n"     // syscall read int
               "ecall\n"
               "mv " + dst + ", a0";
    }

    if (op == "PRINT") {
        std::string src = getRegister(a1);
        return "mv a0, " + src + "\n"
               "li a7, 1\n"
               "ecall\n"
               "la a0, newline\n"
               "li a7, 4\n"
               "ecall";
    }

    return "# Instrucción no soportada: " + op;
}

