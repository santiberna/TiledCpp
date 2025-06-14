#pragma once
#include "tiledcpp/config.hpp"

#include <optional>
#include <variant>

#include "tiledcpp/types/math.hpp"
#include "tiledcpp/types/result.hpp"

namespace rapidxml
{
template <typename T>
class xml_node;
}

namespace tpp
{

struct TILEDCPP_API CustomProperty
{
    std::variant<std::monostate, bool, int, float, std::string, Pixel> value;
};

class TILEDCPP_API PropertyMap
{
public:
    static Result<PropertyMap> fromNode(const rapidxml::xml_node<char>* property_node);

    template <typename T>
    bool has(const std::string& key) const;

    template <typename T>
    void set(const std::string& key, const T& value);

    void remove(const std::string& key);

    template <typename T>
    std::optional<T> get(const std::string& key) const;

    size_t size() const { return data.size(); }

private:
    std::unordered_map<std::string, CustomProperty> data;
};

template <typename T>
bool PropertyMap::has(const std::string& key) const
{
    auto it = data.find(key);
    return it != data.end() && std::holds_alternative<T>(it->second.value);
}

template <typename T>
std::optional<T> PropertyMap::get(const std::string& key) const
{

    auto it = data.find(key);
    if (it == data.end())
        return std::nullopt;

    const auto& var = it->second.value;
    if (!std::holds_alternative<T>(it->second.value))
        return std::nullopt;

    return std::get<T>(var);
}

template <typename T>
void PropertyMap::set(const std::string& key, const T& value)
{
    data[key].value = value;
}

inline void PropertyMap::remove(const std::string& key)
{
    data.erase(key);
}
}