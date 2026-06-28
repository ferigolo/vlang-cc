#pragma once
#include <format>
#include <string>
#include <unordered_map>

#include "DiagnosticEngine.hpp"

enum class ValueType { Int, Float, Vec3, Bool, Unknown };

inline std::string typeToString(ValueType type) {
  switch (type) {
    case ValueType::Int:
      return "Int";
    case ValueType::Float:
      return "Float";
    case ValueType::Vec3:
      return "Vec3";
    case ValueType::Bool:
      return "Bool";
    default:
      return "Unknown";
  }
}

class SymbolTable {
 private:
  std::unordered_map<std::string, ValueType> table;
  DiagnosticEngine& diagEngine;

 public:
  explicit SymbolTable(DiagnosticEngine& diag) : diagEngine(diag) {}

  ValueType lookupVariable(const std::string& name, int line, int column) {
    auto it = table.find(name);
    if (it != table.end()) return it->second;

    diagEngine.report(DiagnosticLevel::Error,
                      std::format("Variable '{}' has not been declared", name),
                      line, column);
    return ValueType::Unknown;
  }

  void assignVariable(const std::string& name, ValueType exprType, int line,
                      int column) {
    auto it = table.find(name);

    if (it != table.end()) {
      // Type Checking
      if (it->second != exprType) {
        diagEngine.report(
            DiagnosticLevel::Error,
            std::format("Attempt to assign {} to variable '{}' of type {}",
                        typeToString(exprType), name, typeToString(it->second)),
            line, column);
      }
    } else {
      table[name] = exprType;
      diagEngine.report(
          DiagnosticLevel::Warning,
          std::format(
              "Variable '{}' was declared implicitly with infered type of {}",
              name, typeToString(exprType)),
          line, column);
    }
  }
};