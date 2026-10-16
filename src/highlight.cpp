#include "grep_cpp/highlight.hpp"

#include <iostream>

namespace {
constexpr std::string_view kMatchStart = "\033[01;31m";
constexpr std::string_view kReset = "\033[m";
} // namespace

void printHighlighted(std::string_view line, std::string_view pattern) {
  if (pattern.empty()) { // avoid an infinite loop on ""
    std::cout << line << '\n';
    return;
  }
  size_t pos = 0;
  size_t hit;
  while ((hit = line.find(pattern, pos)) != std::string_view::npos) {
    std::cout << line.substr(pos, hit - pos) << kMatchStart << pattern
              << kReset;
    pos = hit + pattern.size();
  }
  std::cout << line.substr(pos) << '\n';
}
