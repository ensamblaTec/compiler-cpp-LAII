# 🧠 Compilador - Lenguaje Tipo JAVA con sintaxis español

Este proyecto es un **compilador completo** desarrollado en C++ desde cero para un lenguaje de alto nivel con sintaxis similar a C++, pero con palabras clave en español como `entero`, `texto`, `mostrar`, `entrada`, `si`, `sino`, etc.

## 📦 Estructura del Proyecto

/src/ - Código fuente principal (lexer, parser, ast, symbol table)
include/ - Headers
/output/ - AST exportado en formato JSON
/test/ - Archivos de prueba válidos e inválidos
/build/ - Archivos generados por CMake
/java-interface/ - UI en Java usando JavaFX
main.cpp - Programa principal
CMakeLists.txt - Configuración de compilación

## 🚀 Funcionalidades Principales

### 🔍 Análisis Léxico

- Tokenización con tipos: `IDENTIFIER`, `KEYWORD_INT`, `SEMICOLON`, `ASSIGN`, `LPAREN`, etc.
- Reporte de tokens con fila, columna y valor.
- Tokens generados para todos los tipos de literales, operadores y palabras clave.

### 🧠 Análisis Sintáctico (Parser)

- Generación de AST con soporte completo para:
  - `Declaracion`: declaración de variables con tipo.
  - `Asignacion`: asignación de valores y expresiones.
  - `Mostrar`: impresión de variables o cadenas.
  - `Entrada`: lectura desde consola.
  - `Si` / `Sino`: estructuras condicionales.
  - `Mientras`: bucles while.
  - `Para`: bucles for con inicialización, condición, incremento y bloque.
  - `Bloques`: agrupación de instrucciones.
- Recuperación ante errores usando `synchronize()`.
- Detección de errores como:
  - `sino` sin `si`.
  - Llaves de cierre fuera de bloques.
  - Falta de paréntesis o `;`.

### ✅ Análisis Semántico

- Validación de:
  - Uso de variables no declaradas.
  - Declaración duplicada en el mismo ámbito.
  - Tipos incorrectos en expresiones (`entero + texto`, `bool < 3`, etc.).
  - Uso correcto de tipos booleanos (`si (condición)` debe ser `bool`).
  - Entrada solo sobre variables declaradas.
- Inferencia de tipo automática vía `inferType()`.

### 📚 Tabla de Símbolos

- Manejo de scopes con jerarquía (`global::if_1::while_1`).
- Registro de:
  - Nombre de variable
  - Tipo
  - Valor
  - Línea, columna
  - Ámbito
- Histórico detallado con acciones:
  - Declaraciones
  - Asignaciones
- Exportación a consola al final del análisis.

### 🌲 Árbol de Sintaxis Abstracta (AST)

- Estructura de nodos:
  - `Expression`: binaria, unaria, booleana, variable, número, cadena, asignación.
  - `Statement`: impresión, entrada, asignación, declaración, bloque, condicional, bucle.
- Nodos construidos con `std::shared_ptr<>`.

### 📤 Exportación del AST a JSON

- Exportación con:
  - Tipo de nodo
  - Campos como `name`, `value`, `type`, `condition`, `body`, `left`, `right`.
- Archivo generado en: `output/ast.json`.
- Compatible con visualizadores externos y depuradores.

### 🧪 Logs y Resumen de Errores

- Logs detallados en tiempo real:
  - Tokens generados
  - Entradas al parser
  - Inferencias de tipo
- Sistema de errores:
  - Mensajes amigables y técnicos.
  - Resumen final con número y tipo de errores.

---

## 📂 Ejemplo de Código Fuente

```txt
entero x;
x = 10;

texto mensaje = "Hola";

si (x > 5) {
  mostrar("Mayor a 5");
} sino {
  mostrar("Menor o igual a 5");
}

entrada nombre;
mostrar(nombre);
```
