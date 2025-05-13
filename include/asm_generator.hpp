#pragma once

#include <vector>
#include <string>

#include "ir_optimizer.hpp"

class AsmGenerator {
  public:
    std::string generate(const std::vector<IRInstruction>& tac);
    
  private:
    bool isNumber(const std::string& s);
    // std::string generateHeader();
    // std::string generateFooter();
    std::string generateInstruction(const IRInstruction& instruction);

    std::unordered_map<std::string, std::string> varRegisterMap;
    int tempRegCounter = 0;

    std::string getRegister(const std::string& var);
};
