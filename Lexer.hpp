#pragma once

#include <string_view>
#include <cctype>
#include "Token.hpp"

class Lexer
{
public:
  explicit Lexer(std::string_view src);
  Token getNextToken();

private:
  // std::string_view does not own memory, does not copy strings
  // It points to where the text starts
  std::string_view source; // Zero-Copy Parsing
  size_t position = 0;
  int line = 1;
  int column = 1;

  // Look the next char
  char peek() const;
  char advance();
  void skipWhiteSpace();
};