# Personal Compiler Project

This repository contains the beginnings of a personal compiler project written in C++. Currently, it implements a **Lexer** (lexical analyzer) designed to tokenize a simple C-like programming language.

## Features

- **Zero-Copy Parsing:** Uses `std::string_view` for efficient memory management, avoiding unnecessary string allocations during tokenization.
- **Detailed Token Tracking:** Tracks the line and column numbers of each token for precise error reporting later in the compilation process.
- **Custom Data Types:** Supports basic C types (`int`, `float`) along with vector types (`vec3`), suggesting an orientation toward graphics or physics scripting.

## Project Structure

- `Token.hpp`: Defines the `Token` structure and the `TokenType` enumeration. It lists all the supported keywords (like `int`, `float`, `vec3`, `return`) and symbols (like `+`, `*`, `=`, `==`, `{`, `}`, etc.).
- `Lexer.hpp` & `Lexer.cpp`: Implements the `Lexer` class which consumes the source code string and generates a sequence of tokens (`getNextToken()`).
- `main.cpp`: The entry point of the application.
- `example.code`: An example source file demonstrating the target language syntax.

## Example Language Syntax

Here is an example of the language syntax the lexer is designed to parse (from `example.code`):

```c
int main() {
    vec3 posicao = {1.0, 2.0, 3.0};
    vec3 velocidade = {0.5, 0.0, 0.0};
    float tempo = 10.0;
    
    vec3 destino = posicao + (velocidade * tempo);
    return 0;
}
```

The lexer reads this source code and breaks it down into individual tokens such as identifiers (`posicao`), keywords (`vec3`, `int`), numbers (`1.0`, `10.0`), and operators (`+`, `*`, `=`).
