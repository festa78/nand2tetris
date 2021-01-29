// No copyright.
// Jack code compiler engine.

#ifndef LIB_JACKANALYZER_HPP_
#define LIB_JACKANALYZER_HPP_

#include <memory>
#include <string>

#include "JackTokenizer.hpp"

class JackAnalyzer {
 public:
  explicit JackAnalyzer(const std::string& source,
                        const std::string& output_filename);
  ~JackAnalyzer() = default;

 private:
  std::unique_ptr<JackTokenizer> tokenizer_;
  // std::unique_ptr<CompilationEngine> compilation_engine_;
};

#endif  // LIB_JACKANALYZER_HPP_
