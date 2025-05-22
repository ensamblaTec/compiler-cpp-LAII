#pragma once

#include "ast.hpp"
#include <nlohmann/json.hpp>

#include <memory>
#include <vector>

using json = nlohmann::json;

json expressionToJson(const std::shared_ptr<Expression>& expr);
json statementToJson(const std::shared_ptr<Statement>& stmt);
void exportAstToJsonFile(const std::vector<std::shared_ptr<Statement>>& ast, const std::string& filename);

