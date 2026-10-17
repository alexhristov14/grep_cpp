#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>

#include "grep_cpp/algorithms.hpp"
#include "grep_cpp/file_op.hpp"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << " PATTERN FILE...\n";
    return 2;
  }

  std::string pattern = argv[1];
  std::vector<std::string> files(argv + 2, argv + argc);
  const bool useColor = isatty(fileno(stdout));

  for (const auto &fileName : files) {
    runGrepOnFile(fileName, pattern, boyerMooreGrep, useColor);
  }

  return 0;
}
