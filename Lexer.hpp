#pragma once

#include <cctype>
#include <string_view>

#include "DiagnosticEngine.hpp"
#include "Token.hpp"

class Lexer {
 private:
  // std::string_view does not own memory, does not copy strings
  // It points to where the text starts
  std::string_view source;  // Zero-Copy Parsing
  size_t position = 0;
  int line = 1;
  int column = 1;

  // Look the next char
  char peek() const;
  char advance();
  void skipWhiteSpace();
  TokenType checkKeyword(std::string_view lexeme) const;

  DiagnosticEngine& diagEngine;

 public:
  explicit Lexer(std::string_view src, DiagnosticEngine& diagEng);
  Token getNextToken();
};