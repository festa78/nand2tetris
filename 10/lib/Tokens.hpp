// No copyright.
// Jack tokens.

#ifndef LIB_TOKENS_HPP_
#define LIB_TOKENS_HPP_

#include <string>
#include <vector>

enum class TokenType {
  kKeyWord = 0,
  kSymbol,
  kIdentifier,
  kIntConst,
  kStringConst,
  kFieldSize,
};

enum class KeyWordType {
  kClass = 0,
  kMethod,
  kFunction,
  kConstructor,
  kInt,
  kBoolean,
  kChar,
  kVoid,
  kVar,
  kStatic,
  kField,
  kLet,
  kDo,
  kIf,
  kElse,
  kWhile,
  kReturn,
  kTrue,
  kFalse,
  kNull,
  kThis,
  kFieldSize,
};

const std::vector<std::string> kKeywordNames{
    "class", "method", "function", "constructor", "int",   "boolean", "char",
    "void",  "var",    "static",   "field",       "let",   "do",      "if",
    "else",  "while",  "return",   "true",        "false", "null",    "this",
};

const std::vector<std::string> kSymbols{
    "{", "}", "(", ")", "[", "]", ".", ",", ";", "+",
    "-", "*", "/", "&", "|", "<", ">", "=", "~",
};

class ITokens {
 public:
  virtual ~ITokens() = default;
  virtual bool hasMoreTokens() const = 0;
  virtual void advance() = 0;
  virtual TokenType tokenType() = 0;
  virtual KeyWordType keyWord() = 0;
  virtual std::string symbol() = 0;
  virtual int intVal() = 0;
  virtual std::string stringVal() = 0;
  virtual std::string identifier() = 0;

  virtual void recede() = 0;
};

class Tokens final : public ITokens {
 public:
  Tokens(const std::vector<std::string> tokens);
  Tokens() = delete;
  ~Tokens() = default;

  bool hasMoreTokens() const;
  void advance();
  TokenType tokenType();
  KeyWordType keyWord();
  std::string symbol();
  int intVal();
  std::string stringVal();
  std::string identifier();

  void recede();

 private:
  bool isIntegerConstant(const std::string& token) const;
  bool isValidIdentifier(const std::string& token) const;

  std::vector<std::string> tokens_;
  std::size_t token_index_ = 0;
};

#endif  // LIB_TOKENS_HPP_
