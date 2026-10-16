#pragma once

#include <string>
#include <string_view>
#include <vector>

std::vector<std::string_view> basicGrep(const std::string &buffer,
                                        const std::string &pattern);

std::vector<std::string_view> basicGrep(std::string &&buffer,
                                        const std::string &pattern) = delete;
