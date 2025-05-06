#pragma once

#include "ast.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

json statementToJson(const std::shared_ptr<Statement>& stmt);
json expressionToJson(const std::shared_ptr<Expression>& expr);
