// No copyright.
// Jack code analyzer.

#ifndef LIB_JACKANALYZER_HPP_
#define LIB_JACKANALYZER_HPP_

#include <memory>
#include <string>

#include "CompilationEngine.hpp"
#include "JackTokenizer.hpp"

class JackAnalyzer {
 public:
  explicit JackAnalyzer(const std::string& source,
                        const std::string& output_filename);
  ~JackAnalyzer() = default;

  void compileToXML();

 private:
  const std::string source_;
  const std::string output_filename_;
};

#endif  // LIB_JACKANALYZER_HPP_
