#include "tiledcpp/detail/parse_helpers.hpp"

#include <charconv>
#include <filesystem>
#include <fstream>

std::string tpp::detail::resolvePath(const std::string& base, const std::string& path)
{
    if (std::filesystem::path(path).is_absolute())
    {
        return path;
    }
    else
    {
        std::filesystem::path a { base };
        std::filesystem::path b { path };

        return std::filesystem::weakly_canonical(a / b).string();
    }
}

std::string tpp::detail::getDirectory(const std::string& path)
{
    return std::filesystem::path(path).parent_path().string();
}

std::string tpp::detail::getExtension(const std::string& path)
{
    return std::filesystem::path(path).extension().string();
}

std::string tpp::detail::makePreferredPath(const std::string& path)
{
    return std::filesystem::path(path).make_preferred().string();
}

std::string tpp::detail::getFileContents(const std::string& path)
{
    std::ifstream file { path };

    if (file)
    {
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        std::string out(size, {});
        file.seekg(0);
        file.read(out.data(), size);
        return out;
    }

    return {};
}

std::optional<int> tpp::detail::parseInt(std::string_view view)
{
    int val {};
    auto result = std::from_chars(view.data(), view.data() + view.size(), val, 10);

    if (result.ec == std::errc {})
    {
        return val;
    }
    return std::nullopt;
}

std::optional<float> tpp::detail::parseFloat(std::string_view view)
{
    float val {};
    auto result = std::from_chars(view.data(), view.data() + view.size(), val);

    if (result.ec == std::errc {})
    {
        return val;
    }
    return std::nullopt;
}

std::optional<uint32_t> tpp::detail::parseHex(std::string_view view)
{
    uint32_t val {};
    auto result = std::from_chars(view.data(), view.data() + view.size(), val, 16);

    if (result.ec == std::errc {})
    {
        return val;
    }
    return std::nullopt;
}

std::vector<int> tpp::detail::parseAllInts(std::string_view view)
{
    std::vector<int> out {};

    auto end_it = view.data() + view.size();
    for (auto it = view.data(); it != end_it; ++it)
    {
        if (std::isdigit(*it) || *it == '-')
        {
            std::string_view remaining_view = { it, (size_t)(end_it - it) };

            int val {};
            auto result = std::from_chars(remaining_view.data(), remaining_view.data() + remaining_view.size(), val, 10);

            if (result.ec == std::errc {})
            {
                out.emplace_back(val);
                it = result.ptr;
            }
        }

        if (it == end_it)
            break;
    }

    return out;
}

template <typename T>
std::optional<std::pair<T, ptrdiff_t>> parseNumberWithOffset(std::string_view str, int base)
{
    T val {};
    auto result = std::from_chars(str.data(), str.data() + str.size(), val, base);
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
std::optional<T> parseHex(std::string_view str)
{
    if (auto v = parseNumberWithOffset<T>(str, 16))
    {
        return v->first;
    }
    return std::nullopt;
}

template <typename T>
std::optional<T> parseNumber(std::string_view str)
{
    if (auto v = parseNumberWithOffset<T>(str, 10))
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
            if (auto parsed_num = parseNumberWithOffset<T>(remaining_view, 10))
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