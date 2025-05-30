#pragma once

#include "tiledcpp/config.hpp"

#include <cctype>
#include <charconv>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace tpp::detail
{
TILEDCPP_API std::string getDirectory(const std::string& path);
TILEDCPP_API std::string getExtension(const std::string& path);
TILEDCPP_API std::string resolvePath(const std::string& base, const std::string& path);
TILEDCPP_API std::string makePreferredPath(const std::string& path);
TILEDCPP_API std::string getFileContents(const std::string& path);

TILEDCPP_API std::optional<int> parseInt(std::string_view view);
TILEDCPP_API std::optional<float> parseFloat(std::string_view view);
TILEDCPP_API std::optional<uint32_t> parseHex(std::string_view view);

TILEDCPP_API std::vector<int> parseAllInts(std::string_view view);
}