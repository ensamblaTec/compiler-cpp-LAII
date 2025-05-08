#pragma once

#include "symbol_table.hpp"
#include "ast.hpp"
#include <nlohmann/json.hpp>
#include <memory>

nlohmann::json statementToJson(const std::shared_ptr<Statement>& stmt);
nlohmann::json expressionToJson(const std::shared_ptr<Expression>& expr);

