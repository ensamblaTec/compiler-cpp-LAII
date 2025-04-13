#pragma once

#include <vector>
#include "token.hpp"
#include "lexer.hpp"

std::vector<Token> getAllTokens(Lexer& lexer);
