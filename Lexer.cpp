#include "Lexer.hpp"

Lexer::Lexer(std::string_view src) : source(src) {}

Token Lexer::getNextToken()
{
  skipWhiteSpace();
  if (peek() == '\0')
    return Token{TokenType::EndOfFile, "", line, column};

  char c = advance();

  // Final logic
  return Token{
      TokenType::EndOfFile,
      std::string_view(&source[position - 1], 1), // Memory address and size
      line,
      column};
}

char Lexer::peek() const
{
  if (position >= source.length())
    return '\0'; // Null terminator
  return source[position];
}

char Lexer::advance()
{
  char c = peek();
  position++;
  column++;
  return c;
}

void Lexer::skipWhiteSpace()
{
  while (std::isspace(peek()))
  {
    char c = advance();
    if (c == '\n') // Next line
    {
      line++;
      column = 1;
    }
  }
}
