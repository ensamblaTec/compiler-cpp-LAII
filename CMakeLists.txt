cmake_minimum_required(VERSION 3.10)
project(compiler_app)

set(CMAKE_CXX_STANDARD 17)

# Incluir carpeta de headers
include_directories(include)

# Archivos fuente
add_executable(app
    main.cpp
    src/logger.cpp
    src/lexer.cpp
    src/parser.cpp
    src/utils.cpp
    src/astprinter.cpp
    src/error_reporter.cpp
    src/symbol_table.cpp
)

# Test Lexer
add_executable(test_lexer
    test/lexer/test_lexer.cpp
    src/logger.cpp
    src/lexer.cpp
    src/parser.cpp
    src/utils.cpp
    src/astprinter.cpp
    src/error_reporter.cpp
    src/symbol_table.cpp
)

target_include_directories(test_lexer PRIVATE include)

# Test Parser
add_executable(test_parser
    test/parser/test_parser.cpp
    src/logger.cpp
    src/lexer.cpp
    src/parser.cpp
    src/utils.cpp
    src/astprinter.cpp
    src/error_reporter.cpp
    src/symbol_table.cpp
)

target_include_directories(test_parser PRIVATE include)
