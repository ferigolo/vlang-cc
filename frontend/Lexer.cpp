/*
  Rules:
    1. An identifier must start with letter (a-z, A-Z) or underscore (_)
    2. The next chars might be letters, numbers (0-9) or underscores
*/

#include "Lexer.hpp"

#include <array>
#include <cstdint>
#include <unordered_map>

Lexer::Lexer(std::string_view src, DiagnosticEngine& diagEng)
    : source(src), diagEngine(diagEng) {}

Token Lexer::getNextToken() {
  skipWhiteSpace();

  if (peek() == '\0') return Token{TokenType::EndOfFile, "", line, column};

  // Holds where token starts so we can split the string later
  size_t startPos = position;
  int startCol = column;
  char c = peek();

  // Identifiers and key words
  if (std::isalpha(c) || c == '_') {
    while (std::isalnum(peek()) || peek() == '_') advance();

    // Holds the identifier                 start     start + lexeme lenght
    std::string_view lexeme = source.substr(startPos, position - startPos);
    TokenType type =
        checkKeyword(lexeme);  // it IS a reserved word or a variable name
    return Token{type, lexeme, line, startCol};
  }

  // Identifying numbers
  if (std::isdigit(c)) {
    bool isFloat = false;

    while (std::isdigit(peek())) advance();

    if (peek() == '.') {
      isFloat = true;
      advance();  // Consumes '.'
      while (std::isdigit(peek())) advance();
    }
    std::string_view lexeme = source.substr(startPos, position - startPos);

    // The distinction between int and float is going to be made by the semantic
    // analysis
    return Token{TokenType::Number, lexeme, line, startCol};
  }

  // Operators and punctuation
  switch (c) {
    case '+':
      advance();
      return Token{TokenType::Plus, source.substr(startPos, 1), line, startCol};
    case '*':
      advance();
      return Token{TokenType::Asterisk, source.substr(startPos, 1), line,
                   startCol};
    case '{':
      advance();
      return Token{TokenType::OpenBrace, source.substr(startPos, 1), line,
                   startCol};
    case '}':
      advance();
      return Token{TokenType::CloseBrace, source.substr(startPos, 1), line,
                   startCol};
    case '(':
      advance();
      return Token{TokenType::OpenParen, source.substr(startPos, 1), line,
                   startCol};
    case ')':
      advance();
      return Token{TokenType::CloseParen, source.substr(startPos, 1), line,
                   startCol};
    case ';':
      advance();
      return Token{TokenType::Semicolon, source.substr(startPos, 1), line,
                   startCol};
    case ',':
      advance();
      return Token{TokenType::Comma, source.substr(startPos, 1), line,
                   startCol};
    case '=':
      advance();  // Consumes first '='
      if (peek() == '=') {
        advance();  // Consumes second '='
        return Token{TokenType::Equals, source.substr(startPos, 2), line,
                     startCol};
      }
      return Token{TokenType::Assign, source.substr(startPos, 1), line,
                   startCol};
    case '<':
      advance();
      if (peek() == '=') {
        advance();
        return Token{TokenType::LessEqual, source.substr(startPos, 2), line,
                     startCol};
      }
      return Token{TokenType::LessThan, source.substr(startPos, 1), line,
                   startCol};

    case '>':
      advance();
      if (peek() == '=') {
        advance();
        return Token{TokenType::GreaterEqual, source.substr(startPos, 2), line,
                     startCol};
      }
      return Token{TokenType::GreaterThan, source.substr(startPos, 1), line,
                   startCol};

    case '!':
      advance();
      if (peek() == '=') {
        advance();
        return Token{TokenType::NotEquals, source.substr(startPos, 2), line,
                     startCol};
      }
      break;
  }

  // Let the parser handles erros or unknown simbols - this groups all erros
  // together and keep the parser responsible for errors (SOLID)
  advance();  // Consumes the unknown character to prevent infinite loops
  diagEngine.report(DiagnosticLevel::Error, "Unknown symbol encountered.", line,
                    column);
  return Token{TokenType::Unknown, source.substr(startPos, 1), line, column};
}

char Lexer::peek() const {
  if (position >= source.length()) return '\0';  // Null terminator
  return source[position];
}

char Lexer::advance() {
  char c = peek();
  position++;
  column++;
  return c;
}

void Lexer::skipWhiteSpace() {
  while (std::isspace(peek())) {
    char c = advance();
    if (c == '\n')  // Next line
    {
      line++;
      column = 1;
    }
  }
}

TokenType Lexer::checkKeyword(std::string_view lexeme) const {
  static const std::unordered_map<std::string_view, TokenType> keywords = {
      {"int", TokenType::KeywordInt},
      {"float", TokenType::KeywordFloat},
      {"vec3", TokenType::KeywordVec3},
      {"return", TokenType::KeywordReturn}};

  auto it = keywords.find(lexeme);
  if (it != keywords.end()) return it->second;  // Reserved key word

  return TokenType::Identifier;  // Variable name defined by user
}

struct KeywordEntry {
  std::string_view word;
  TokenType type;
};

// Zero overhead
// MUST be in alphabetical order
constexpr std::array<KeywordEntry, 4> keywords = {
    {{"float", TokenType::KeywordFloat},
     {"int", TokenType::KeywordInt},
     {"return", TokenType::KeywordReturn},
     {"vec3", TokenType::KeywordVec3}}};

constexpr TokenType checkKeywordConstexpr(std::string_view lexeme) {
  int left = 0;
  int right = static_cast<int>(keywords.size()) - 1;

  while (left <= right) {
    int mid = left + (right - left) / 2;
    // constexpr compare
    // Returns 0 if equals, < 0 if lexeme comes before, > 0 if lexeme comes
    // after
    int cmp = lexeme.compare(keywords[mid].word);

    if (cmp == 0)
      return keywords[mid].type;  // Found reserved key work
    else if (cmp < 0)
      right = mid - 1;  // Descart right half
    else
      left = mid + 1;  //  Descart left half
  }

  return TokenType::Identifier;  // Not reserved key word
}