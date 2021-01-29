// No copyright.
// Jack code tokenizer.

#ifndef LIB_JACKTOKENIZER_HPP_
#define LIB_JACKTOKENIZER_HPP_

#include <fstream>
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

enum class KeyWord {
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

class JackTokenizer {
 public:
  JackTokenizer(const std::string& input_filename,
                const std::string& output_filename);
  ~JackTokenizer() = default;

  void parseInputFile() noexcept;

  bool hasMoreTokens() const noexcept;
  void advance();
  TokenType tokenType();
  KeyWord keyWord();
  std::string symbol();
  int intVal();
  std::string stringVal();
  std::string identifier();

  // Exposed for tests.
  std::vector<std::string> getTokens() const noexcept;
  void setTokens(const std::vector<std::string>& tokens) noexcept;
  std::vector<std::string> getCodeLinesFromFile();
  std::vector<std::string> getTokensFromCodeLines(
      std::vector<std::string> code_lines);

 private:
  bool isIntegerConstant(const std::string& token) const;
  bool isValidIdentifier(const std::string& token) const;

  std::string extractJackCode(const std::string& line) const;
  std::string trimSpaceFromLine(const std::string& line) const noexcept;

  std::vector<std::string> getTokensFromCodeLine(const std::string& code_line);
  std::vector<std::string> separateCodeLineBySpace(
      const std::string& code_line);
  std::vector<std::size_t> findSymbolIndices(
      const std::string& line) const noexcept;

  std::vector<std::string> tokens_;
  std::size_t token_index_ = 0;

  std::string input_filename_;
  const std::string& output_filename_;
};

#endif  // LIB_JACKTOKENIZER_HPP_
