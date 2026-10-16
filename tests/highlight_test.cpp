#include "grep_c++/highlight.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>

namespace {
// Runs printHighlighted and returns what it wrote to std::cout.
std::string captureHighlighted(std::string_view line,
                               std::string_view pattern) {
  std::ostringstream out;
  auto *old = std::cout.rdbuf(out.rdbuf());
  printHighlighted(line, pattern);
  std::cout.rdbuf(old);
  return out.str();
}
} // namespace

TEST(PrintHighlighted, ColorsSingleMatch) {
  EXPECT_EQ(captureHighlighted("say hello", "hello"),
            "say \033[01;31mhello\033[m\n");
}

TEST(PrintHighlighted, ColorsEveryMatch) {
  EXPECT_EQ(captureHighlighted("ab ab", "ab"),
            "\033[01;31mab\033[m \033[01;31mab\033[m\n");
}

TEST(PrintHighlighted, NoMatchPrintsLineUnchanged) {
  EXPECT_EQ(captureHighlighted("foo", "bar"), "foo\n");
}

TEST(PrintHighlighted, EmptyPatternPrintsLineUnchanged) {
  EXPECT_EQ(captureHighlighted("foo", ""), "foo\n");
}
