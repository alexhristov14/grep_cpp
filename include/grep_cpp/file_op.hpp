#pragma once

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

using GrepFunc = std::vector<std::string_view> (*)(const std::string &buffer,
                                                   const std::string &pattern);

void runGrepOnFile(const std::string &fileName, const std::string &pattern,
                   GrepFunc func, bool useColor, bool ignoreCase = false);
