#include "jsonexporter.hpp"

using json = nlohmann::json;

json expressionToJson(const std::shared_ptr<Expression>& expr) {
    if (!expr) return nullptr;

    if (auto num = std::dynamic_pointer_cast<NumberExpr>(expr)) {
        return {
            {"type", "NumberExpr"},
            {"value", num->value}
        };
    }

    if (auto str = std::dynamic_pointer_cast<StringExpr>(expr)) {
        return {
            {"type", "StringExpr"},
            {"value", str->text}
        };
    }

    if (auto b = std::dynamic_pointer_cast<BooleanExpr>(expr)) {
        return {
            {"type", "BooleanExpr"},
            {"value", b->value}
        };
    }

    if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr)) {
        return {
            {"type", "VariableExpr"},
            {"name", var->name}
        };
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
            {"expr", expressionToJson(un->right)}
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

    if (auto exprStmt = std::dynamic_pointer_cast<ExpressionStatement>(stmt)) {
        return {
            {"type", "ExpressionStatement"},
            {"expression", expressionToJson(exprStmt->expression)}
        };
    }

    if (auto ifStmt = std::dynamic_pointer_cast<IfStatement>(stmt)) {
        return {
            {"type", "IfStatement"},
            {"condition", expressionToJson(ifStmt->condition)},
            {"thenBranch", statementToJson(ifStmt->thenBranch)},
            {"elseBranch", statementToJson(ifStmt->elseBranch)}
        };
    }

    if (auto whileStmt = std::dynamic_pointer_cast<WhileStatement>(stmt)) {
        return {
            {"type", "WhileStatement"},
            {"condition", expressionToJson(whileStmt->condition)},
            {"body", statementToJson(whileStmt->body)}
        };
    }

    if (auto forStmt = std::dynamic_pointer_cast<ForStatement>(stmt)) {
        return {
            {"type", "ForStatement"},
            {"initializer", statementToJson(forStmt->init)},
            {"condition", expressionToJson(forStmt->condition)},
            {"increment", expressionToJson(forStmt->increment)},
            {"body", statementToJson(forStmt->body)}
        };
    }

    if (auto printStmt = std::dynamic_pointer_cast<PrintStatement>(stmt)) {
        return {
            {"type", "PrintStatement"},
            {"value", expressionToJson(printStmt->expr)}
        };
    }

    if (auto inputStmt = std::dynamic_pointer_cast<InputStatement>(stmt)) {
        return {
            {"type", "InputStatement"},
            {"varName", inputStmt->variable}
        };
    }

    if (auto block = std::dynamic_pointer_cast<BlockStatement>(stmt)) {
        json stmtsJson = json::array();
        for (auto& s : block->statements) {
            stmtsJson.push_back(statementToJson(s));
        }
        return {
            {"type", "BlockStatement"},
            {"statements", stmtsJson}
        };
    }

    return { {"type", "UnknownStatement"} };
}

