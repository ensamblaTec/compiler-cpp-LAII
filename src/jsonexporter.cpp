#include "jsonexporter.hpp"
#include <fstream>

json expressionToJson(const std::shared_ptr<Expression>& expr) {
    if (!expr) return nullptr;

    if (auto num = std::dynamic_pointer_cast<NumberExpr>(expr)) {
        return { {"type", "NumberExpr"}, {"value", num->value} };
    }
    if (auto str = std::dynamic_pointer_cast<StringExpr>(expr)) {
        return { {"type", "StringExpr"}, {"text", str->text} };
    }
    if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr)) {
        return { {"type", "VariableExpr"}, {"name", var->name} };
    }
    if (auto bin = std::dynamic_pointer_cast<BinaryExpr>(expr)) {
        return {
            {"type", "BinaryExpr"},
            {"op", bin->op},
            {"left", expressionToJson(bin->left)},
            {"right", expressionToJson(bin->right)}
        };
    }
    if (auto un = std::dynamic_pointer_cast<UnaryExpr>(expr)) {
        return {
            {"type", "UnaryExpr"},
            {"op", un->op},
            {"right", expressionToJson(un->right)}
        };
    }
    if (auto bl = std::dynamic_pointer_cast<BooleanExpr>(expr)) {
        return {
            {"type", "BooleanExpr"},
            {"value", bl->value}
        };
    }
    if (auto assign = std::dynamic_pointer_cast<AssignmentExpr>(expr)) {
        return {
            {"type", "AssignmentExpr"},
            {"name", assign->name},
            {"value", expressionToJson(assign->value)}
        };
    }

    return { {"type", "UnknownExpr"} };
}

json statementToJson(const std::shared_ptr<Statement>& stmt) {
    if (!stmt) return nullptr;

    if (auto decl = std::dynamic_pointer_cast<Declaration>(stmt)) {
        return {
            {"type", "Declaration"},
            {"varType", decl->type},
            {"name", decl->name}
        };
    }
    if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt)) {
        return {
            {"type", "Assignment"},
            {"name", assign->name},
            {"value", expressionToJson(assign->expr)}
        };
    }
    if (auto input = std::dynamic_pointer_cast<InputStatement>(stmt)) {
        return {
            {"type", "InputStatement"},
            {"variable", input->variable}
        };
    }
    if (auto print = std::dynamic_pointer_cast<PrintStatement>(stmt)) {
        return {
            {"type", "PrintStatement"},
            {"expr", expressionToJson(print->expr)}
        };
    }
    if (auto ifstmt = std::dynamic_pointer_cast<IfStatement>(stmt)) {
        return {
            {"type", "IfStatement"},
            {"condition", expressionToJson(ifstmt->condition)},
            {"thenBranch", statementToJson(ifstmt->thenBranch)},
            {"elseBranch", statementToJson(ifstmt->elseBranch)}
        };
    }
    if (auto whilestmt = std::dynamic_pointer_cast<WhileStatement>(stmt)) {
        return {
            {"type", "WhileStatement"},
            {"condition", expressionToJson(whilestmt->condition)},
            {"body", statementToJson(whilestmt->body)}
        };
    }
    if (auto forstmt = std::dynamic_pointer_cast<ForStatement>(stmt)) {
        return {
            {"type", "ForStatement"},
            {"init", statementToJson(forstmt->init)},
            {"condition", expressionToJson(forstmt->condition)},
            {"increment", statementToJson(forstmt->increment)},
            {"body", statementToJson(forstmt->body)}
        };
    }
    if (auto block = std::dynamic_pointer_cast<BlockStatement>(stmt)) {
        json blockJson = json::array();
        for (const auto& inner : block->statements) {
            blockJson.push_back(statementToJson(inner));
        }
        return {
            {"type", "BlockStatement"},
            {"statements", blockJson}
        };
    }
    if (auto exprstmt = std::dynamic_pointer_cast<ExpressionStatement>(stmt)) {
        return {
            {"type", "ExpressionStatement"},
            {"expression", expressionToJson(exprstmt->expression)}
        };
    }

    return { {"type", "UnknownStatement"} };
}

void exportAstToJsonFile(const std::vector<std::shared_ptr<Statement>>& statements, const std::string& filename) {
    json root = json::array();
    for (const auto& stmt : statements) {
        root.push_back(statementToJson(stmt));
    }

    std::ofstream file(filename);
    if (file.is_open()) {
        file << root.dump(4);
        file.close();
    }
}

