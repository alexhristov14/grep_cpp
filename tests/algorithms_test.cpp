#include "grep_c++/algorithms.hpp"

#include <gtest/gtest.h>

#include <string>

TEST(BasicGrep, ReturnsMatchingLines) {
  const std::string buffer = "hello world\nfoo\nsay hello\n";
  auto results = basicGrep(buffer, "hello");
  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0], "hello world");
  EXPECT_EQ(results[1], "say hello");
}

TEST(BasicGrep, NoMatches) {
  const std::string buffer = "foo\nbar\n";
  EXPECT_TRUE(basicGrep(buffer, "baz").empty());
}

TEST(BasicGrep, LastLineWithoutTrailingNewline) {
  const std::string buffer = "foo\nbar";
  auto results = basicGrep(buffer, "bar");
  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0], "bar");
}

TEST(BasicGrep, EmptyBuffer) {
  const std::string buffer;
  EXPECT_TRUE(basicGrep(buffer, "x").empty());
}

TEST(BasicGrep, IsCaseSensitive) {
  const std::string buffer = "Hello\n";
  EXPECT_TRUE(basicGrep(buffer, "hello").empty());
}
