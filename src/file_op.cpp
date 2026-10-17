#include "grep_cpp/file_op.hpp"
#include "grep_cpp/highlight.hpp"

#include <cctype>

static std::string toLowerASCII(std::string_view in) {
  std::string out(in);

  for (auto &c : out) {
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  }

  return out;
}

void runGrepOnFile(const std::string &fileName, const std::string &pattern,
                   GrepFunc func, bool useColor, bool ignoreCase) {
  std::ifstream file(fileName, std::ios::binary);

  if (file.is_open()) {
    std::stringstream buffer;
    buffer << file.rdbuf();
    const std::string content = buffer.str();

    const std::string searchContent =
        ignoreCase ? toLowerASCII(content) : std::string();
    const std::string searchPattern =
        ignoreCase ? toLowerASCII(pattern) : std::string();
    const std::string &haystack = ignoreCase ? searchContent : content;
    const std::string &needle = ignoreCase ? searchPattern : pattern;

    auto results = func(haystack, needle);

    for (const auto &result : results) {
      const auto offset =
          static_cast<std::size_t>(result.data() - haystack.data());
      std::string_view original(content.data() + offset, result.size());

      if (useColor)
        printHighlighted(original, result, needle);
      else
        std::cout << original << '\n';
    }

    file.close();
  } else {
    std::cerr << "Unable to open file '" + fileName + "'\n";
  }
}
