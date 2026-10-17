#pragma once

#include <string_view>

void printHighlighted(std::string_view line, std::string_view pattern);

void printHighlighted(std::string_view line, std::string_view searchLine,
                      std::string_view pattern);
