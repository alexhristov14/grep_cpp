#include "grep_c++/algorithms.hpp"

std::vector<std::string_view> basicGrep(const std::string &buffer,
                                        const std::string &pattern) {
  std::vector<std::string_view> results;
  size_t start = 0;

  while (start < buffer.size()) {
    size_t end = buffer.find('\n', start);
    if (end == std::string::npos)
      end = buffer.size(); // last line without trailing newline

    std::string_view line(buffer.data() + start, end - start);
    if (line.find(pattern) != std::string_view::npos)
      results.push_back(line);

    start = end + 1;
  }

  return results;
}
