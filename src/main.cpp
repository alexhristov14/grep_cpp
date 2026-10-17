#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>

#include "grep_cpp/algorithms.hpp"
#include "grep_cpp/file_op.hpp"

int main(int argc, char *argv[]) {
  bool ignoreCase = false; // -i
  int nextArg = 1;

  if (nextArg < argc && std::string(argv[nextArg]) == "-i") {
    ignoreCase = true;
    ++nextArg;
  }

  if (argc - nextArg < 2) {
    std::cerr << "usage: " << argv[0] << " [options]" << " PATTERN FILE...\n";
    return 2;
  }

  std::string pattern = argv[nextArg++];
  std::vector<std::string> files(argv + nextArg, argv + argc);
  const bool useColor = isatty(fileno(stdout));

  for (const auto &fileName : files) {
    runGrepOnFile(fileName, pattern, boyerMooreGrep, useColor, ignoreCase);
  }

  return 0;
}
