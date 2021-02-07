// No copyright.
// Main program code.

#include <iostream>
#include <stdexcept>

#include "lib/JackAnalyzer.hpp"

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cout << "Invalid num of inputs" << std::endl;
    return 1;
  }

  const std::string source(argv[1]);
  const std::string output_filename(argv[2]);

  try {
    JackAnalyzer analyzer(source, output_filename);
    analyzer.compileToXML();
  } catch (std::runtime_error e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}
