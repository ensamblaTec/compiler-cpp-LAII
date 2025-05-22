#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "ir_generator.hpp"
#include "ir_optimizer.hpp"

class AsmGenerator {
public:
  std::string generate(const std::vector<IRInstruction>& instructions);
  std::string compile(const std::vector<IRInstruction>& original_instructions);

private:
  std::string generate_asm_riscv(const std::vector<IRInstruction>& instructions);
  std::string getImmediateReg(const std::string& value, std::ostringstream& out);
  bool isLabel(const std::string& s);
  bool isNumber(const std::string& s);
  std::string getTempName(const std::string& label);
  std::string getRegister(const std::string& name);
  std::string getRegisterTemp(const std::string& name);
  void preprocessTAC(std::vector<IRInstruction>& instructions);

  int tempCounter = 0;
  int registerCounter = 0;
  std::unordered_map<std::string, std::string> tempMapping;
  std::unordered_map<std::string, std::string> varRegister;
};

