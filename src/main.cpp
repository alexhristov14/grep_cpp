#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <unistd.h>

#include "grep_cpp/algorithms.hpp"
#include "grep_cpp/highlight.hpp"

int main(int argc, char *argv[]) {
  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << " PATTERN FILE...\n";
    return 2;
  }

  std::string pattern = argv[1];
  std::vector<std::string> files(argv + 2, argv + argc);
  const bool useColor = isatty(fileno(stdout));

  for (const auto &fileName : files) {
    std::ifstream file(fileName, std::ios::binary);

    if (file.is_open()) {
      std::stringstream buffer;
      buffer << file.rdbuf();
      std::string content = buffer.str();

      auto results = boyerMooreGrep(content, pattern);

      for (const auto &result : results) {
        if (useColor)
          printHighlighted(result, pattern);
        else
          std::cout << result << '\n';
      }

      file.close();
    } else {
      std::cerr << "Unable to open file '" + fileName + "'\n";
    }
  }

  return 0;
}
