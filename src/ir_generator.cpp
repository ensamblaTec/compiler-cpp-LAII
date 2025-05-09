#include "ir_generator.hpp"

std::vector<IRInstruction> IRGenerator::generateFromAST(const std::vector<std::shared_ptr<Statement>>& statements) {
    instructions.clear();
    tempCounter = 0;
    for (const auto& stmt : statements) {
        generateFromStatement(stmt);
    }
    return instructions;
}

void IRGenerator::generateFromStatement(const std::shared_ptr<Statement>& stmt) {
    if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
        std::string temp = generateFromExpression(assign->expr);
        instructions.emplace_back("ASSIGN", temp, "", assign->name);
    }
    else if (auto print = std::dynamic_pointer_cast<PrintStatement>(stmt)) {
        std::string temp = generateFromExpression(print->expr);
        instructions.emplace_back("PRINT", temp, "", "");
    }
    else if (auto block = std::dynamic_pointer_cast<BlockStatement>(stmt)) {
        for (const auto& s : block->statements) {
            generateFromStatement(s);
        }
    }
    else if (auto ifStmt = std::dynamic_pointer_cast<IfStatement>(stmt)) {
        std::string cond = generateFromExpression(ifStmt->condition);
        std::string labelElse = newTemp();
        std::string labelEnd = newTemp();
        instructions.emplace_back("IF_FALSE_GOTO", cond, "", labelElse);
        generateFromStatement(ifStmt->thenBranch);
        instructions.emplace_back("GOTO", "", "", labelEnd);
        instructions.emplace_back("LABEL", labelElse, "", "");
        if (ifStmt->elseBranch) {
            generateFromStatement(ifStmt->elseBranch);
        }
        instructions.emplace_back("LABEL", labelEnd, "", "");
    }
    else if (auto whileStmt = std::dynamic_pointer_cast<WhileStatement>(stmt)) {
        std::string labelStart = newTemp();
        std::string labelEnd = newTemp();
        instructions.emplace_back("LABEL", labelStart, "", "");
        std::string cond = generateFromExpression(whileStmt->condition);
        instructions.emplace_back("IF_FALSE_GOTO", cond, "", labelEnd);
        generateFromStatement(whileStmt->body);
        instructions.emplace_back("GOTO", "", "", labelStart);
        instructions.emplace_back("LABEL", labelEnd, "", "");
    }
    else if (auto forStmt = std::dynamic_pointer_cast<ForStatement>(stmt)) {
        generateFromStatement(forStmt->init);
        std::string labelCond = newTemp();
        std::string labelEnd = newTemp();
        instructions.emplace_back("LABEL", labelCond, "", "");
        std::string cond = generateFromExpression(forStmt->condition);
        instructions.emplace_back("IF_FALSE_GOTO", cond, "", labelEnd);
        generateFromStatement(forStmt->body);
        generateFromStatement(forStmt->increment);
        instructions.emplace_back("GOTO", "", "", labelCond);
        instructions.emplace_back("LABEL", labelEnd, "", "");
    }
}

std::string IRGenerator::generateFromExpression(const std::shared_ptr<Expression>& expr) {
    if (auto num = std::dynamic_pointer_cast<NumberExpr>(expr)) {
        return num->value;
    }
    if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr)) {
        return var->name;
    }
    if (auto bin = std::dynamic_pointer_cast<BinaryExpr>(expr)) {
        std::string left = generateFromExpression(bin->left);
        std::string right = generateFromExpression(bin->right);
        std::string temp = newTemp();
        instructions.emplace_back(bin->op, left, right, temp);
        return temp;
    }
    if (auto boolean = std::dynamic_pointer_cast<BooleanExpr>(expr)) {
        return boolean->value ? "1" : "0";
    }
    if (auto str = std::dynamic_pointer_cast<StringExpr>(expr)) {
        return str->text;
    }
    return "error";
}

std::string IRGenerator::newTemp() {
    return "t" + std::to_string(++tempCounter);
}

