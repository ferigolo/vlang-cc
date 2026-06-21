#pragma once
#include <stdexcept>
#include <string>

class CompilerError : public std::runtime_error
{
private:
  int line, column;

public:
  CompilerError(const std::string &message, int l, int c) : std::runtime_error(message), line(l), column(c) {};
  int getLine() const { return line; }
  int getColumn() const { return column; }

  std::string getFormatedMessage() const
  {
    return "[SINTATIC ERROR - Line " + std::to_string(line) + ", Column " + std::to_string(column) + "]: " + what();
  }
};