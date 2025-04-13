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
)
