#pragma once
#include <format>
#include <string>
#include <unordered_map>

#include "DiagnosticEngine.hpp"

enum class ValueType { Int, Float, Vect3, Unknown };

class SymbolTable {
 private:
  std::unordered_map<std::string, ValueType> table;
  DiagnosticEngine& diagEngine;

 public:
  void defineVariable(const std::string& name, ValueType type, int line,
                      int collumn) {
    if (table.find(name) != table.end())
      diagEngine.report(DiagnosticLevel::Error,
                        std::format("Variable '{}' was already declared", name),
                        line, collumn);
  }

  ValueType lookupVariable(const std::string& name, int line,
                           int collumn) const {
    auto it = table.find(name);
    if (it != table.end())
      return it->second;
    else
      diagEngine.report(
          DiagnosticLevel::Error,
          std::format("Variable '{}' has not been declared", name), line,
          collumn);
  }
};