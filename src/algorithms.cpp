#include "grep_cpp/algorithms.hpp"
#include <algorithm>
#include <cstring>
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

static constexpr std::size_t NUM_BYTES = 256;

static std::array<std::size_t, NUM_BYTES>
buildShiftTable(std::string_view pattern) {
  const std::size_t m = pattern.size();
  std::array<std::size_t, NUM_BYTES> shift;
  shift.fill(m);

  // the last character is excluded so a match never shifts by 0
  for (std::size_t i = 0; i + 1 < m; ++i) {
    shift[static_cast<unsigned char>(pattern[i])] = m - 1 - i;
  }

  return shift;
}

static constexpr std::size_t RARE_BYTE_SAMPLE = 4096;

// Picks the pattern byte that is least frequent in the start of the buffer, so
// memchr on it stops on as few false candidates as possible.
static std::size_t findRareByteIndex(std::string_view buffer,
                                     std::string_view pattern) {
  std::array<std::size_t, NUM_BYTES> freq{};
  const std::size_t sample = std::min(buffer.size(), RARE_BYTE_SAMPLE);
  for (std::size_t i = 0; i < sample; ++i)
    ++freq[static_cast<unsigned char>(buffer[i])];

  std::size_t best = pattern.size() - 1;
  for (std::size_t i = 0; i < pattern.size(); ++i) {
    if (freq[static_cast<unsigned char>(pattern[i])] <
        freq[static_cast<unsigned char>(pattern[best])])
      best = i;
  }

  return best;
}

std::vector<std::string_view> boyerMooreGrep(const std::string &buffer,
                                             const std::string &pattern) {
  if (pattern.empty())
    return basicGrep(buffer, pattern); // every line matches

  std::vector<std::string_view> results;

  const std::size_t n = buffer.size();
  const std::size_t m = pattern.size();
  std::size_t pos = 0;

  const auto shift = buildShiftTable(pattern);
  const std::size_t rare = findRareByteIndex(buffer, pattern);
  const char *data = buffer.data();

  while (m <= n && pos <= n - m) {
    // let memchr (SIMD) skip ahead to the next alignment where the rare byte
    // lines up; the last valid alignment is n - m
    const void *hit =
        std::memchr(data + pos + rare, pattern[rare], n - m + 1 - pos);
    if (hit == nullptr)
      break;
    pos = static_cast<const char *>(hit) - data - rare;

    if (std::memcmp(data + pos, pattern.data(), m) == 0) {
      std::size_t prev = getPreviousNewline(buffer, pos);
      std::size_t lineStart = (prev == std::string_view::npos) ? 0 : prev + 1;

      std::size_t lineEnd = buffer.find('\n', pos);
      if (lineEnd == std::string::npos)
        lineEnd = buffer.size(); // last line without trailing newline

      results.emplace_back(buffer.data() + lineStart, lineEnd - lineStart);

      pos = lineEnd + 1; // skip the rest of the matched line
      continue;
    }

    pos += shift[static_cast<unsigned char>(buffer[pos + m - 1])];
  }

  return results;
}
