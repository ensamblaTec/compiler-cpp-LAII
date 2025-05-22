#include "asm_generator.hpp"

#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <cctype>
#include <algorithm>
#include <iostream>
#include <set>
#include <regex>
#include <fstream>

bool AsmGenerator::isLabel(const std::string& s) {
    return std::regex_match(s, std::regex("l[0-9]+"));
}

bool AsmGenerator::isNumber(const std::string& s) {
    return std::regex_match(s, std::regex("[0-9]+"));
}

std::string AsmGenerator::getImmediateReg(const std::string& value, std::ostringstream& out) {
    if (!isNumber(value)) return getRegisterTemp(value);

    std::string immReg = "t" + std::to_string(registerCounter++);
    out << "li " << immReg << ", " << value << "\n";
    return immReg;
}

std::string AsmGenerator::getTempName(const std::string& label) {
    if (tempMapping.count(label)) return tempMapping[label];
    std::string newName = "t" + std::to_string(tempCounter++);
    tempMapping[label] = newName;
    return newName;
}

std::string AsmGenerator::getRegister(const std::string& name) {
    if (!varRegister.count(name)) {
        varRegister[name] = "t" + std::to_string(registerCounter++);
    }
    return varRegister[name];
}

std::string AsmGenerator::getRegisterTemp(const std::string& name) {
    if (varRegister.count(name)) return varRegister[name];
    return name;
}

void AsmGenerator::preprocessTAC(std::vector<IRInstruction>& instructions) {
    std::unordered_map<std::string, std::string> renameLog;

    for (auto& inst : instructions) {
        const std::string& op = inst.op;

        if (op == "LABEL") continue;

        if (isLabel(inst.arg1) && !(op == "GOTO" || op == "IF_FALSE_GOTO")) {
            std::string old = inst.arg1;
            inst.arg1 = getTempName(inst.arg1);
            renameLog[old] = inst.arg1;
            std::cout << "[preprocessTAC] Renombrando arg1: " << old << " -> " << inst.arg1 << "\n";
        }

        if (isLabel(inst.arg2)) {
            std::string old = inst.arg2;
            inst.arg2 = getTempName(inst.arg2);
            renameLog[old] = inst.arg2;
            std::cout << "[preprocessTAC] Renombrando arg2: " << old << " -> " << inst.arg2 << "\n";
        }

        if (isLabel(inst.result) && !(op == "GOTO" || op == "IF_FALSE_GOTO")) {
            std::string old = inst.result;
            inst.result = getTempName(inst.result);
            renameLog[old] = inst.result;
            std::cout << "[preprocessTAC] Renombrando result: " << old << " -> " << inst.result << "\n";
        }
    }

    std::cout << "\n=== Mapeo de etiquetas renombradas ===\n";
    for (const auto& [oldName, newName] : renameLog) {
        std::cout << "Etiqueta " << oldName << " → " << newName << "\n";
    }
    std::cout << "======================================\n\n";
}

std::string AsmGenerator::generate_asm_riscv(const std::vector<IRInstruction>& instructions) {
    bool useSyscalls = true;

    std::ostringstream out;
    std::unordered_set<std::string> variables;
    std::unordered_map<std::string, std::string> addrMap;
    int addrCounter = 0;

    auto isVariable = [this](const std::string& s) {
        return !s.empty() && !isLabel(s) && !isNumber(s);
    };

    for (const auto& inst : instructions) {
        if (isVariable(inst.result)) variables.insert(inst.result);
        if (isVariable(inst.arg1)) variables.insert(inst.arg1);
        if (isVariable(inst.arg2)) variables.insert(inst.arg2);
    }

    for (const auto& var : variables) {
        addrMap[var] = "s" + std::to_string(addrCounter++);
    }

    out << ".section .data\n";
    for (const auto& var : variables) {
        out << var << ": .word 0\n";
    }

    out << "\n.section .rodata\n";
    out << "end_msg: .asciz \"terminado\\n\"\n";

    out << "\n.section .text\n";
    out << ".globl my_logic\n";
    out << "my_logic:\n";

    for (const auto& [var, sreg] : addrMap) {
        if (!isNumber(var)) {
            out << "    la " << sreg << ", " << var << "\n";
        }
    }

    for (const auto& inst : instructions) {
        const std::string& op = inst.op;
        const std::string& a1 = inst.arg1;
        const std::string& a2 = inst.arg2;
        const std::string& res = inst.result;

        if (op == "INPUT") {
            out << "    li a7, 5\n";
            out << "    ecall\n";
            out << "    sw a0, 0(" << addrMap[res] << ")\n";
        }
        else if (op == "PRINT") {
            out << "    lw a0, 0(" << addrMap[a1] << ")\n";
            out << "    li a7, 1\n";
            out << "    ecall\n";
            out << "    li a0, 10\n";
            out << "    li a7, 11\n";
            out << "    ecall\n";
        }
        else if (op == "ASSIGN") {
            if (variables.count(a1))
                out << "    lw t0, 0(" << addrMap[a1] << ")\n";
            else
                out << "    li t0, " << a1 << "\n";
            out << "    sw t0, 0(" << addrMap[res] << ")\n";
        }
        else if (op == "+") {
            out << "    lw t0, 0(" << addrMap[a1] << ")\n";
            if (variables.count(a2))
                out << "    lw t1, 0(" << addrMap[a2] << ")\n";
            else
                out << "    li t1, " << a2 << "\n";
            out << "    add t2, t0, t1\n";
            out << "    sw t2, 0(" << addrMap[res] << ")\n";
        }
        else if (op == "<") {
            out << "    lw t0, 0(" << addrMap[a1] << ")\n";
            if (variables.count(a2))
                out << "    lw t1, 0(" << addrMap[a2] << ")\n";
            else
                out << "    li t1, " << a2 << "\n";
            out << "    slt t2, t0, t1\n";
            out << "    sw t2, 0(" << addrMap[res] << ")\n";
        }
        else if (op == "==") {
            out << "    lw t0, 0(" << addrMap[a1] << ")\n";
            if (variables.count(a2))
                out << "    lw t1, 0(" << addrMap[a2] << ")\n";
            else
                out << "    li t1, " << a2 << "\n";
            out << "    xor t2, t0, t1\n";
            out << "    seqz t2, t2\n";
            out << "    sw t2, 0(" << addrMap[res] << ")\n";
        }
        else if (op == "IF_FALSE_GOTO") {
            out << "    lw t0, 0(" << addrMap[a1] << ")\n";
            out << "    beqz t0, " << res << "\n";
        }
        else if (op == "GOTO") {
            out << "    j " << res << "\n";
        }
        else if (op == "LABEL") {
            std::string label = res.empty() ? a1 : res;
            if (!label.empty()) {
                out << label << ":\n";
            }
        }
    }

    out << "    la a0, end_msg\n";
    out << "    li a7, 93\n";
    out << "    li a0, 0\n";
    out << "    ecall\n";

    return out.str();
}

std::string AsmGenerator::generate(const std::vector<IRInstruction>& instructions) {
    
    std::ostringstream out;
    out << ".data\n";
    out << "newline: .asciz \"\\n\"\n\n";
    out << ".text\n";
    out << ".globl _start\n";
    out << "_start:\n";
    out << "  j main\n\n";
    out << ".globl main\n";
    out << "main:\n";

    std::vector<IRInstruction> processed = instructions;
    preprocessTAC(processed);

    for (const auto& inst : processed) {
        const auto& op = inst.op;
        const auto& a1 = inst.arg1;
        const auto& a2 = inst.arg2;
        const auto& res = inst.result;

        if (op == "LABEL") {
          std::string label = res.empty() ? a1 : res;
          out << label << ":\n";
        }
        else if (op == "ASSIGN") {
            if (isNumber(a1)) {
                out << "li " << getRegister(res) << ", " << a1 << "\n";
            } else {
                out << "mv " << getRegister(res) << ", " << getRegisterTemp(a1) << "\n";
            }
        }
        else if (op == "<") {
            std::string left = getImmediateReg(a1, out);
            std::string right = getImmediateReg(a2, out);
            out << "slt " << getRegister(res) << ", " << left << ", " << right << "\n";
        }
        else if (op == "==") {
            std::string left = getImmediateReg(a1, out);
            std::string right = getImmediateReg(a2, out);
            std::string reg = getRegister(res);
            out << "xor " << reg << ", " << left << ", " << right << "\n";
            out << "seqz " << reg << ", " << reg << "\n";
        }
        else if (op == "+") {
            std::string left = getImmediateReg(a1, out);
            std::string right = getImmediateReg(a2, out);
            out << "add " << getRegister(res) << ", " << left << ", " << right << "\n";
        }
        else if (op == "IF_FALSE_GOTO") {
            std::string condReg = getRegisterTemp(a1);
            if (isLabel(res)) {
                out << "beqz " << condReg << ", " << res << "\n";
            } else {
                out << "# ⚠ IF_FALSE_GOTO con destino no etiqueta: " << res << "\n";
            }
        }
        else if (op == "GOTO") {
            if (isLabel(res)) {
                out << "j " << res << "\n";
            } else {
                out << "# ⚠ GOTO con destino no etiqueta: " << res << "\n";
            }
        }
        else if (op == "PRINT") {
            out << "mv a0, " << getRegisterTemp(a1) << "\n";
            out << "li a7, 1\n";
            out << "ecall\n";
            out << "la a0, newline\n";
            out << "li a7, 4\n";
            out << "ecall\n";
        }
        else if (op == "INPUT") {
            std::string reg = getRegister(a1);
            out << "li a7, 5\n";
            out << "ecall\n";
            out << "addi " << reg << ", a0, 0\n";
        }
        else {
            out << "# ⚠ Instrucción no soportada: " << op << "\n";
        }
    }

    out << "\nli a7, 10     # syscall: exit\n";
    out << "ecall\n";
    return out.str();
}

std::string AsmGenerator::compile(const std::vector<IRInstruction>& original_instructions) {
    std::vector<IRInstruction> instructions = original_instructions;
    preprocessTAC(instructions);
    std::string asm_code = generate_asm_riscv(instructions);

    return asm_code;
}
