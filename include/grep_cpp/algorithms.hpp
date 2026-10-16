#pragma once

#include <array>
#include <string>
#include <string_view>
#include <vector>

std::vector<std::string_view> basicGrep(const std::string &buffer,
                                        const std::string &pattern);

std::vector<std::string_view> basicGrep(std::string &&buffer,
                                        const std::string &pattern) = delete;

std::vector<std::string_view> onePassGrep(const std::string &buffer,
                                          const std::string &pattern);

std::vector<std::string_view> onePassGrep(std::string &&buffer,
                                          const std::string &pattern) = delete;

// Boyer-Moore-Horspool with a memchr prefilter on the pattern's rarest byte:
// sublinear on average, O(n * m) worst case, O(1) space
std::vector<std::string_view> boyerMooreGrep(const std::string &buffer,
                                             const std::string &pattern);

std::vector<std::string_view>
boyerMooreGrep(std::string &&buffer, const std::string &pattern) = delete;
