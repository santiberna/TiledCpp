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

std::optional<uint32_t> tpp::detail::parseIndex(std::string_view view)
{
    uint32_t val {};
    auto result = std::from_chars(view.data(), view.data() + view.size(), val, 10);

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

std::vector<uint32_t> tpp::detail::parseCSV(std::string_view view)
{
    std::vector<uint32_t> out {};

    auto next_comma = view.find_first_of(',');
    while (next_comma != std::string_view::npos)
    {
        std::string_view token = view.substr(0, next_comma);

        if (token.front() == '\n')
        {
            token.remove_prefix(1);
        }

        if (auto parsed_num = parseIndex(token))
        {
            out.emplace_back(parsed_num.value());
        }

        view.remove_prefix(next_comma + 1);
        next_comma = view.find_first_of(',');
    }

    if (!view.empty())
    {
        if (view.front() == '\n')
        {
            view.remove_prefix(1);
        }

        if (auto parsed_num = parseIndex(view))
        {
            out.emplace_back(parsed_num.value());
        }
    }

    return out;
}