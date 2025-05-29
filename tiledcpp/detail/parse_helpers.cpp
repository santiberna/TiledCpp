#include "tiledcpp/detail/parse_helpers.hpp"
#include <filesystem>

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