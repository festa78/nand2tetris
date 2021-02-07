// No copyright.
// Jack code tokenizer.

#ifndef LIB_JACKTOKENIZER_HPP_
#define LIB_JACKTOKENIZER_HPP_

#include <memory>
#include <string>
#include <vector>

#include "lib/Tokens.hpp"

class IJackTokenizer {
 public:
  virtual ~IJackTokenizer() = default;
  virtual Tokens parseInputFile() = 0;
};

class JackTokenizer final : public IJackTokenizer {
 public:
  explicit JackTokenizer(const std::string& input_filename);
  JackTokenizer() = delete;
  ~JackTokenizer() = default;

  Tokens parseInputFile() noexcept;

 private:
  std::vector<std::string> getCodeLinesFromFile();
  std::vector<std::string> getTokensFromCodeLines(
      std::vector<std::string> code_lines);

  std::string extractJackCode(const std::string& line) const;
  std::string trimSpaceFromLine(const std::string& line) const noexcept;

  std::vector<std::string> getTokensFromCodeLine(const std::string& code_line);
  std::vector<std::string> separateCodeLineBySpace(
      const std::string& code_line);
  std::vector<std::size_t> findSymbolIndices(
      const std::string& line) const noexcept;

  std::string input_filename_;
};

#endif  // LIB_JACKTOKENIZER_HPP_
