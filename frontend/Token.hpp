#include <string_view>
#include <variant>

enum class TokenType {
  Identifier,     // variable names
  Number,         // 1.0, 2.0, 10
  KeywordInt,     // int
  KeywordFloat,   // float
  KeywordVec3,    // vec3
  KeywordReturn,  // return
  KeywordIf,      // If
  KeywordElse,    // Else
  Plus,           // +
  Minus,          // -
  Asterisk,       // *
  Slash,          // /
  Assign,         // =
  Equals,         // ==
  NotEquals,      // !=
  LessThan,       // <
  LessEqual,      // <=
  GreaterThan,    // >
  GreaterEqual,   // >=
  OpenBrace,      // {
  CloseBrace,     // }
  OpenParen,      // (
  CloseParen,     // )
  Semicolon,      // ;
  Comma,          // ,
  EndOfFile,
  Unknown
};

struct Token {
  TokenType type;
  std::string_view lexeme;
  // Error handling
  int line;
  int column;
};
