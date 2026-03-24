#pragma once
#include <string>

// ── Sapphire Error Codes (hex-range) ──────────────────────────────────────
// Format: E<category><hex-digit><hex-digit>
//
//   E1xx  Syntax     (E100–E1FF)
//   E2xx  Name       (E200–E2FF)
//   E3xx  Type       (E300–E3FF)
//   E4xx  Runtime    (E400–E4FF)
//   E5xx  Import     (E500–E5FF)
//   E6xx  IO         (E600–E6FF)
//   E7xx  Value      (E700–E7FF)
//
// Hex digits mean each category has 256 slots (0x00–0xFF).
// e.g. E1A0, E2B3 are valid future codes.

namespace sapphire {

struct ErrorCode {
    const char* code;
    const char* title;
    const char* note;
    const char* docs_url;
};

// ── Syntax (E1xx) ──────────────────────────────────────────────────────────
inline constexpr ErrorCode E1A0 = {
    "E1A0", "unexpected token",
    "check for missing colons, parentheses, or mismatched brackets",
    "https://sapphire-lang.org/errors#E1A0"
};
inline constexpr ErrorCode E1A1 = {
    "E1A1", "expected expression",
    "an expression (value, variable, or function call) was expected here",
    "https://sapphire-lang.org/errors#E1A1"
};
inline constexpr ErrorCode E1A2 = {
    "E1A2", "invalid indentation",
    "Sapphire uses 4-space indentation — check for mixed tabs/spaces",
    "https://sapphire-lang.org/errors#E1A2"
};
inline constexpr ErrorCode E1A3 = {
    "E1A3", "unterminated string",
    "add a closing quote to end the string literal",
    "https://sapphire-lang.org/errors#E1A3"
};
inline constexpr ErrorCode E1A4 = {
    "E1A4", "invalid decorator placement",
    "decorators can only be applied to functions and classes",
    "https://sapphire-lang.org/errors#E1A4"
};
inline constexpr ErrorCode E1A5 = {
    "E1A5", "missing colon",
    "fn, class, if, for, and while bodies must end with ':'",
    "https://sapphire-lang.org/errors#E1A5"
};
inline constexpr ErrorCode E1A6 = {
    "E1A6", "invalid assignment target",
    "only variables and subscript expressions can be assigned to",
    "https://sapphire-lang.org/errors#E1A6"
};
inline constexpr ErrorCode E1A7 = {
    "E1A7", "body must be indented on the next line",
    "Sapphire does not allow single-line bodies — put the body on the next line, indented",
    "https://sapphire-lang.org/errors#E1A7"
};

// ── Name (E2xx) ────────────────────────────────────────────────────────────
inline constexpr ErrorCode E2A0 = {
    "E2A0", "undefined variable",
    "check the spelling, or make sure the variable is defined before use",
    "https://sapphire-lang.org/errors#E2A0"
};
inline constexpr ErrorCode E2A1 = {
    "E2A1", "undefined function",
    "check the function name, or make sure the module is imported",
    "https://sapphire-lang.org/errors#E2A1"
};
inline constexpr ErrorCode E2A2 = {
    "E2A2", "undefined module",
    "check the module name — use 'import <module>' at the top of the file",
    "https://sapphire-lang.org/errors#E2A2"
};
inline constexpr ErrorCode E2A3 = {
    "E2A3", "attribute not found",
    "check the attribute name, or verify the object type",
    "https://sapphire-lang.org/errors#E2A3"
};

// ── Type (E3xx) ────────────────────────────────────────────────────────────
inline constexpr ErrorCode E3A0 = {
    "E3A0", "type mismatch",
    "the value type does not match what was expected",
    "https://sapphire-lang.org/errors#E3A0"
};
inline constexpr ErrorCode E3A1 = {
    "E3A1", "unsupported operand types",
    "this operator cannot be used with these types",
    "https://sapphire-lang.org/errors#E3A1"
};
inline constexpr ErrorCode E3A2 = {
    "E3A2", "not callable",
    "only functions and classes can be called with ()",
    "https://sapphire-lang.org/errors#E3A2"
};
inline constexpr ErrorCode E3A3 = {
    "E3A3", "not iterable",
    "for loops require a list, string, range, or other iterable",
    "https://sapphire-lang.org/errors#E3A3"
};
inline constexpr ErrorCode E3A4 = {
    "E3A4", "not subscriptable",
    "only lists, strings, and dicts support [] indexing",
    "https://sapphire-lang.org/errors#E3A4"
};

// ── Runtime (E4xx) ─────────────────────────────────────────────────────────
inline constexpr ErrorCode E4A0 = {
    "E4A0", "index out of bounds",
    "the index is outside the valid range — check the list length first",
    "https://sapphire-lang.org/errors#E4A0"
};
inline constexpr ErrorCode E4A1 = {
    "E4A1", "division by zero",
    "the divisor evaluated to zero — add a check before dividing",
    "https://sapphire-lang.org/errors#E4A1"
};
inline constexpr ErrorCode E4A2 = {
    "E4A2", "assertion failed",
    "an assert() condition was false — check the logic being asserted",
    "https://sapphire-lang.org/errors#E4A2"
};
inline constexpr ErrorCode E4A3 = {
    "E4A3", "null dereference",
    "the value is 'none' — check for none before accessing it",
    "https://sapphire-lang.org/errors#E4A3"
};
inline constexpr ErrorCode E4A4 = {
    "E4A4", "stack overflow",
    "infinite or deeply recursive function — check your base case",
    "https://sapphire-lang.org/errors#E4A4"
};

// ── Import (E5xx) ──────────────────────────────────────────────────────────
inline constexpr ErrorCode E5A0 = {
    "E5A0", "module not found",
    "check the module name spelling and that it is installed",
    "https://sapphire-lang.org/errors#E5A0"
};
inline constexpr ErrorCode E5A1 = {
    "E5A1", "circular import",
    "two modules are importing each other — restructure to break the cycle",
    "https://sapphire-lang.org/errors#E5A1"
};

// ── IO (E6xx) ──────────────────────────────────────────────────────────────
inline constexpr ErrorCode E6A0 = {
    "E6A0", "file not found",
    "check the file path and that the file exists",
    "https://sapphire-lang.org/errors#E6A0"
};
inline constexpr ErrorCode E6A1 = {
    "E6A1", "permission denied",
    "the process does not have permission to access this file",
    "https://sapphire-lang.org/errors#E6A1"
};

// ── Value (E7xx) ───────────────────────────────────────────────────────────
inline constexpr ErrorCode E7A0 = {
    "E7A0", "invalid argument",
    "a function received an argument with an unexpected value",
    "https://sapphire-lang.org/errors#E7A0"
};
inline constexpr ErrorCode E7A1 = {
    "E7A1", "argument count mismatch",
    "check the number of arguments the function expects",
    "https://sapphire-lang.org/errors#E7A1"
};
inline constexpr ErrorCode E7A2 = {
    "E7A2", "value out of range",
    "the value exceeds the allowed range for this operation",
    "https://sapphire-lang.org/errors#E7A2"
};

// ── Lookup ─────────────────────────────────────────────────────────────────
inline const ErrorCode* lookup_error_code(const std::string& code) {
    static const ErrorCode* all[] = {
        &E1A0,&E1A1,&E1A2,&E1A3,&E1A4,&E1A5,&E1A6,&E1A7,
        &E2A0,&E2A1,&E2A2,&E2A3,
        &E3A0,&E3A1,&E3A2,&E3A3,&E3A4,
        &E4A0,&E4A1,&E4A2,&E4A3,&E4A4,
        &E5A0,&E5A1,
        &E6A0,&E6A1,
        &E7A0,&E7A1,&E7A2,
        nullptr
    };
    for (int i = 0; all[i]; i++)
        if (code == all[i]->code) return all[i];
    return nullptr;
}

} // namespace sapphire
