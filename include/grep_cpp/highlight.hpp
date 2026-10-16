#pragma once

#include <string_view>

// Prints `line` to stdout with every occurrence of `pattern` colored,
// followed by a newline.
void printHighlighted(std::string_view line, std::string_view pattern);
