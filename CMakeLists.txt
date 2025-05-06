cmake_minimum_required(VERSION 3.10)
project(compiler_app)

set(CMAKE_CXX_STANDARD 17)

# Archivos fuente
set(SOURCES
    main.cpp
    src/logger.cpp
    src/lexer.cpp
    src/parser.cpp
    src/utils.cpp
    src/astprinter.cpp
    src/error_reporter.cpp
    src/symbol_table.cpp
    src/jsonexporter.cpp
)

# Incluir headers
include_directories(include)

# ============================
# 1. Ejecutable Linux/macOS
# ============================
add_executable(app ${SOURCES})

# ============================
# 2. Ejecutable Windows (app.exe)
# ============================
if(UNIX AND NOT APPLE)
    find_program(MINGW_GPP NAMES x86_64-w64-mingw32-g++)

    if (MINGW_GPP)
        message(STATUS "✅ Cross-compiler encontrado: ${MINGW_GPP}")

        set(WIN_EXE "${CMAKE_CURRENT_BINARY_DIR}/app.exe")

        add_custom_target(app_exe ALL
            COMMAND ${CMAKE_COMMAND} -E echo "🔧 Compilando app.exe con mingw..."
            COMMAND ${CMAKE_COMMAND} -E remove -f ${WIN_EXE}
            COMMAND ${MINGW_GPP} -static -std=c++17 -O2 -Iinclude -o ${WIN_EXE} ${SOURCES}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "✅ app.exe generado correctamente"
            VERBATIM
        )
    else()
        message(WARNING "❌ mingw-w64 no encontrado. No se generará app.exe")
    endif()
endif()

# ============================
# 3. Limpieza manual
# ============================
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/app
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_CURRENT_BINARY_DIR}/app.exe
    COMMENT "🧹 Limpiando ejecutables"
)

# Crear carpeta dist y mover app (Linux/macOS:)
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/build/dist
    COMMAND ${CMAKE_COMMAND} -E rename $<TARGET_FILE:app> ${CMAKE_SOURCE_DIR}/build/dist/app
    COMMENT "📦 app copiado a dist/"
)

# Mover app.exe si existe
if(MINGW_GPP)
    add_custom_command(TARGET app_exe POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/build/dist
        COMMAND ${CMAKE_COMMAND} -E copy ${WIN_EXE} ${CMAKE_SOURCE_DIR}/build/dist/app.exe
        COMMENT "📦 app.exe copiado a dist/"
    )
endif()


