#include "jsonexporter.hpp"
#include "ast.hpp"

using json = nlohmann::json;

json expressionToJson(const std::shared_ptr<Expression>& expr) {
    if (!expr) return nullptr;

    if (auto num = std::dynamic_pointer_cast<NumberExpr>(expr))
        return {{"type", "NumberExpr"}, {"value", num->value}};

    if (auto str = std::dynamic_pointer_cast<StringExpr>(expr))
        return {{"type", "StringExpr"}, {"text", str->text}};

    if (auto boolean = std::dynamic_pointer_cast<BooleanExpr>(expr))
        return {{"type", "BooleanExpr"}, {"value", boolean->value}};

    if (auto var = std::dynamic_pointer_cast<VariableExpr>(expr))
        return {{"type", "VariableExpr"}, {"name", var->name}};

    if (auto bin = std::dynamic_pointer_cast<BinaryExpr>(expr))
        return {
            {"type", "BinaryExpr"},
            {"op", bin->op},
            {"left", expressionToJson(bin->left)},
            {"right", expressionToJson(bin->right)}
        };

    if (auto un = std::dynamic_pointer_cast<UnaryExpr>(expr))
        return {
            {"type", "UnaryExpr"},
            {"op", un->op},
            {"right", expressionToJson(un->right)}
        };

    return {{"type", "UnknownExpression"}};
}

json statementToJson(const std::shared_ptr<Statement>& stmt) {
    if (!stmt) return nullptr;

    if (auto printStmt = std::dynamic_pointer_cast<PrintStatement>(stmt))
        return {
            {"type", "PrintStatement"},
            {"value", expressionToJson(printStmt->expression)}
        };

    if (auto inputStmt = std::dynamic_pointer_cast<InputStatement>(stmt))
        return {
            {"type", "InputStatement"},
            {"varName", inputStmt->name}
        };

    if (auto assign = std::dynamic_pointer_cast<Assignment>(stmt))
        return {
            {"type", "Assignment"},
            {"name", assign->name},
            {"value", expressionToJson(assign->value)}
        };

    if (auto decl = std::dynamic_pointer_cast<Declaration>(stmt))
        return {
            {"type", "Declaration"},
            {"varType", decl->varType},
            {"name", decl->name}
        };

    if (auto block = std::dynamic_pointer_cast<BlockStatement>(stmt)) {
        json stmts = json::array();
        for (const auto& s : block->statements)
            stmts.push_back(statementToJson(s));
        return {
            {"type", "BlockStatement"},
            {"statements", stmts}
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
            {"increment", statementToJson(forStmt->increment)},
            {"body", statementToJson(forStmt->body)}
        };
    }

    return {{"type", "UnknownStatement"}};
}

