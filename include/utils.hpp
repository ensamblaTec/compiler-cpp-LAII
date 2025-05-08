#pragma once

#include <vector>
#include <string>
#include <fstream>
#include "token.hpp"
#include "lexer.hpp"
#include "symbol_table.hpp"
#include "jsonexporter.hpp"
#include "ast.hpp"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

std::vector<Token> getAllTokens(Lexer& lexer);
std::string symbolTableToString(std::vector<Token> tokens);
bool saveFile(std::string fileName, std::string content);
void createFolder(const char* folderName);
void exportSymbolTableToJson(const SymbolTable& table);
void exportAstToFile(const std::vector<std::shared_ptr<Statement>>& statements, const std::string& outputPath);
