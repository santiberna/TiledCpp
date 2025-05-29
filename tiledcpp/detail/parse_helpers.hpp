#pragma once

#include "tiledcpp/tiledcpp.hpp"

#include <cctype>
#include <charconv>
#include <fstream>
#include <optional>
#include <string_view>
#include <vector>

namespace tpp::detail
{

TILEDCPP_API std::string getDirectory(const std::string& path);
TILEDCPP_API std::string getExtension(const std::string& path);
TILEDCPP_API std::string resolvePath(const std::string& base, const std::string& path);
TILEDCPP_API std::string makePreferredPath(const std::string& path);
TILEDCPP_API std::string getFileContents(const std::string& path);

// TODO: MOVE IMPLEMENTATIONS TO END OF THIS FILE

template <typename T>
std::optional<std::pair<T, ptrdiff_t>> parseNumberWithOffset(std::string_view str)
{
    T val {};
    auto result = std::from_chars(str.data(), str.data() + str.size(), val);
    if (result.ec == std::errc {})
    {
        ptrdiff_t diff = result.ptr - str.data();
        return std::make_pair(val, diff);
    }
    else
    {
        return std::nullopt;
    }
}

template <typename T>
std::optional<T> parseNumber(std::string_view str)
{
    if (auto v = parseNumberWithOffset<T>(str))
    {
        return v->first;
    }
    return std::nullopt;
}

template <typename T>
std::vector<T> parseAllNumbers(std::string_view str)
{
    std::vector<T> out {};

    auto end_it = str.end();
    for (auto it = str.begin(); it != end_it; ++it)
    {
        if (std::isdigit(*it) || *it == '-')
        {
            std::string_view remaining_view = { &*it, (size_t)(end_it - it) };
            if (auto parsed_num = parseNumberWithOffset<T>(remaining_view))
            {
                it += parsed_num.value().second;
                out.emplace_back(parsed_num.value().first);
            }
        }

        if (it == end_it)
            break;
    }

    return out;
}

}