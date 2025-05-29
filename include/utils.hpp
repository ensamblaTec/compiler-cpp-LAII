#pragma once

#include <vector>
#include <string>
#include <fstream>
#include "token.hpp"
#include "lexer.hpp"
#include "symbol_table.hpp"
#include "ast.hpp"

std::vector<Token> getAllTokens(Lexer& lexer);
void tokensToCSV(std::vector<Token> tokens);
bool saveFile(std::string fileName, std::string content);
void createFolder(const char* folderName);

