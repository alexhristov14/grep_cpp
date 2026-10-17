#include <gtest/gtest.h>

#include "grep_cpp/algorithms.hpp"
#include "grep_cpp/file_op.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace {
// Redirects a stream's buffer for the lifetime of the object.
class StreamCapture {
public:
  explicit StreamCapture(std::ostream &stream)
      : stream_(stream), old_(stream.rdbuf(out_.rdbuf())) {}
  ~StreamCapture() { stream_.rdbuf(old_); }
  std::string str() const { return out_.str(); }

private:
  std::ostream &stream_;
  std::ostringstream out_;
  std::streambuf *old_;
};

// Records what runGrepOnFile hands to the grep function.
std::string gSeenBuffer;
std::string gSeenPattern;
int gCalls = 0;

std::vector<std::string_view> recordingGrep(const std::string &buffer,
                                            const std::string &pattern) {
  gSeenBuffer = buffer;
  gSeenPattern = pattern;
  ++gCalls;
  return {};
}

class RunGrepOnFile : public ::testing::Test {
protected:
  void SetUp() override {
    const auto *info = ::testing::UnitTest::GetInstance()->current_test_info();
    path_ = std::filesystem::temp_directory_path() /
            (std::string("grep_cpp_") + info->name() + ".txt");
    gSeenBuffer.clear();
    gSeenPattern.clear();
    gCalls = 0;
  }

  void TearDown() override { std::filesystem::remove(path_); }

  void writeFile(const std::string &content) {
    std::ofstream file(path_, std::ios::binary);
    file << content;
  }

  // Runs runGrepOnFile and returns what it wrote to std::cout.
  std::string run(const std::string &pattern, GrepFunc func = onePassGrep,
                  bool useColor = false) {
    StreamCapture out(std::cout);
    runGrepOnFile(path_.string(), pattern, func, useColor);
    return out.str();
  }

  std::filesystem::path path_;
};
} // namespace

TEST_F(RunGrepOnFile, PrintsMatchingLines) {
  writeFile("hello world\nfoo\nsay hello\n");
  EXPECT_EQ(run("hello"), "hello world\nsay hello\n");
}

TEST_F(RunGrepOnFile, NoMatchesPrintsNothing) {
  writeFile("foo\nbar\n");
  EXPECT_EQ(run("baz"), "");
}

TEST_F(RunGrepOnFile, EmptyFilePrintsNothing) {
  writeFile("");
  EXPECT_EQ(run("x"), "");
}

TEST_F(RunGrepOnFile, LastLineWithoutTrailingNewline) {
  writeFile("foo\nbar");
  EXPECT_EQ(run("bar"), "bar\n");
}

TEST_F(RunGrepOnFile, UseColorHighlightsMatches) {
  writeFile("say hello\nfoo\n");
  EXPECT_EQ(run("hello", onePassGrep, true), "say \033[01;31mhello\033[m\n");
}

TEST_F(RunGrepOnFile, WorksWithEveryAlgorithm) {
  writeFile("alpha\nbeta\nalphabet\n");
  const GrepFunc funcs[] = {basicGrep, onePassGrep, boyerMooreGrep};
  for (GrepFunc func : funcs) {
    EXPECT_EQ(run("alpha", func), "alpha\nalphabet\n");
  }
}

TEST_F(RunGrepOnFile, PassesWholeFileAndPatternToGrepFunc) {
  const std::string content = "line one\r\nline two\n\xff\xfe binary\n";
  writeFile(content);
  run("needle", recordingGrep);
  EXPECT_EQ(gCalls, 1);
  EXPECT_EQ(gSeenBuffer, content); // read in binary mode, CRLF untouched
  EXPECT_EQ(gSeenPattern, "needle");
}

TEST_F(RunGrepOnFile, PreservesEmbeddedNulBytes) {
  const std::string content("a\0b\nc\n", 6);
  writeFile(content);
  run("b", recordingGrep);
  EXPECT_EQ(gSeenBuffer, content);
}

TEST_F(RunGrepOnFile, MissingFileReportsErrorAndSkipsGrep) {
  StreamCapture err(std::cerr);
  const std::string out = run("x", recordingGrep);
  EXPECT_EQ(out, "");
  EXPECT_EQ(gCalls, 0);
  EXPECT_EQ(err.str(), "Unable to open file '" + path_.string() + "'\n");
}
