#include "grep_cpp/algorithms.hpp"

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

TEST(OnePassGrep, ReturnsMatchingLines) {
  const std::string buffer = "hello world\nfoo\nsay hello\n";
  auto results = onePassGrep(buffer, "hello");
  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0], "hello world");
  EXPECT_EQ(results[1], "say hello");
}

TEST(OnePassGrep, NoMatches) {
  const std::string buffer = "foo\nbar\n";
  EXPECT_TRUE(onePassGrep(buffer, "baz").empty());
}

TEST(OnePassGrep, LastLineWithoutTrailingNewline) {
  const std::string buffer = "foo\nbar";
  auto results = onePassGrep(buffer, "bar");
  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0], "bar");
}

TEST(OnePassGrep, EmptyBuffer) {
  const std::string buffer;
  EXPECT_TRUE(onePassGrep(buffer, "x").empty());
}

TEST(OnePassGrep, IsCaseSensitive) {
  const std::string buffer = "Hello\n";
  EXPECT_TRUE(onePassGrep(buffer, "hello").empty());
}

TEST(OnePassGrep, MultipleMatchesOnOneLineReturnLineOnce) {
  const std::string buffer = "foo foo\nbar\nfoo\n";
  auto results = onePassGrep(buffer, "foo");
  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0], "foo foo");
  EXPECT_EQ(results[1], "foo");
}

TEST(OnePassGrep, MatchOnFirstLine) {
  const std::string buffer = "needle\nhay\n";
  auto results = onePassGrep(buffer, "needle");
  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0], "needle");
}

TEST(OnePassGrep, MatchesBasicGrep) {
  const std::string buffers[] = {
      "hello world\nfoo\nsay hello\n",
      "foo foo\nbar\nfoo",
      "\n\nfoo\n\n",
      "no match here\n",
      "",
  };
  const std::string patterns[] = {"hello", "foo", "o", "x", ""};

  for (const auto &buffer : buffers) {
    for (const auto &pattern : patterns) {
      EXPECT_EQ(onePassGrep(buffer, pattern), basicGrep(buffer, pattern))
          << "buffer: \"" << buffer << "\" pattern: \"" << pattern << "\"";
    }
  }
}

TEST(BoyerMooreGrep, ReturnsMatchingLines) {
  const std::string buffer = "hello world\nfoo\nsay hello\n";
  auto results = boyerMooreGrep(buffer, "hello");
  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0], "hello world");
  EXPECT_EQ(results[1], "say hello");
}

TEST(BoyerMooreGrep, MultipleMatchesOnOneLineReturnLineOnce) {
  const std::string buffer = "foo foo\nbar\nfoo\n";
  auto results = boyerMooreGrep(buffer, "foo");
  ASSERT_EQ(results.size(), 2u);
  EXPECT_EQ(results[0], "foo foo");
  EXPECT_EQ(results[1], "foo");
}

TEST(BoyerMooreGrep, PatternLongerThanBuffer) {
  const std::string buffer = "ab";
  EXPECT_TRUE(boyerMooreGrep(buffer, "abc").empty());
}

TEST(BoyerMooreGrep, NonAsciiBytes) {
  const std::string buffer = "caf\xc3\xa9\nplain\n";
  auto results = boyerMooreGrep(buffer, "\xc3\xa9");
  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0], "caf\xc3\xa9");
}

TEST(BoyerMooreGrep, MatchesBasicGrep) {
  const std::string buffers[] = {
      "hello world\nfoo\nsay hello\n",
      "foo foo\nbar\nfoo",
      "\n\nfoo\n\n",
      "no match here\n",
      "aaaaab\naab\nab\n",
      "",
  };
  const std::string patterns[] = {"hello", "foo", "o", "x", "aab", "ab", ""};

  for (const auto &buffer : buffers) {
    for (const auto &pattern : patterns) {
      EXPECT_EQ(boyerMooreGrep(buffer, pattern), basicGrep(buffer, pattern))
          << "buffer: \"" << buffer << "\" pattern: \"" << pattern << "\"";
    }
  }
}

TEST(BoyerMooreGrep, MatchAtEndOfBufferWhenRareByteIsFirst) {
  // 'q' never appears in the sample, so it becomes the memchr anchor
  const std::string buffer = std::string(5000, 'e') + "\nqee";
  auto results = boyerMooreGrep(buffer, "qee");
  ASSERT_EQ(results.size(), 1u);
  EXPECT_EQ(results[0], "qee");
}

TEST(BoyerMooreGrep, RareByteNearEndDoesNotReadPastBuffer) {
  // the anchor byte occurs where the full pattern would overrun the buffer
  const std::string buffer = "xxxxq";
  EXPECT_TRUE(boyerMooreGrep(buffer, "qxx").empty());
}
