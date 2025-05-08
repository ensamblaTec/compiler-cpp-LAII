#pragma once

#include "symbol_table.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::string categoryToString(SymbolCategory cat);

void to_json(json& j, const Symbol& sym);
