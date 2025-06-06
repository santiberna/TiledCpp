#pragma once
#include "tiledcpp/config.hpp"

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
    std::unordered_map<std::string, CustomProperty> data;
};
}