#include "grep_cpp/file_op.hpp"
#include "grep_cpp/highlight.hpp"

void runGrepOnFile(const std::string &fileName, const std::string &pattern,
                   GrepFunc func, bool useColor) {
  std::ifstream file(fileName, std::ios::binary);

  if (file.is_open()) {
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    auto results = func(content, pattern);

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
