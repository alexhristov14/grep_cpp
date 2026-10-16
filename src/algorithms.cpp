#include "grep_cpp/algorithms.hpp"
#include <string_view>

static std::size_t getPreviousNewline(std::string_view buffer,
                                      std::size_t pos) {
  if (pos == 0 || buffer.empty()) {
    return std::string_view::npos;
  }

  return buffer.rfind('\n', pos - 1);
}

std::vector<std::string_view> basicGrep(const std::string &buffer,
                                        const std::string &pattern) {
  std::vector<std::string_view> results;
  std::size_t start = 0;

  while (start < buffer.size()) {
    std::size_t end = buffer.find('\n', start);
    if (end == std::string::npos)
      end = buffer.size(); // last line without trailing newline

    std::string_view line(buffer.data() + start, end - start);
    if (line.find(pattern) != std::string_view::npos)
      results.push_back(line);

    start = end + 1;
  }

  return results;
}

std::vector<std::string_view> onePassGrep(const std::string &buffer,
                                          const std::string &pattern) {
  std::vector<std::string_view> results;
  std::size_t start = 0;
  std::size_t next = buffer.find(pattern, start);

  while (start < buffer.size() && next != std::string::npos) {
    std::size_t prev = getPreviousNewline(buffer, next);
    std::size_t lineStart = (prev == std::string_view::npos) ? 0 : prev + 1;

    std::size_t lineEnd = buffer.find('\n', next);
    if (lineEnd == std::string::npos)
      lineEnd = buffer.size(); // last line without trailing newline

    results.emplace_back(buffer.data() + lineStart, lineEnd - lineStart);

    start = lineEnd + 1;
    next = buffer.find(pattern, start);
  }

  return results;
}
