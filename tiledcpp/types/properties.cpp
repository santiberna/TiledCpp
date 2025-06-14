
#include "tiledcpp/types/properties.hpp"
#include "tiledcpp/detail/parse_helpers.hpp"

#include <RapidXML/rapidxml.hpp>

tpp::Result<std::pair<std::string, tpp::CustomProperty>> parseCustomProperty(const rapidxml::xml_node<char>* prop_node)
{
    auto* name_attrib = prop_node->first_attribute("name");
    auto* val_attrib = prop_node->first_attribute("value");

    if (name_attrib == nullptr || val_attrib == nullptr)
    {
        tpp::Error err { "[Custom Property] could not find name or value attributes" };
        return tl::make_unexpected(err);
    }

    std::string name = name_attrib->value();
    std::string val = val_attrib->value();

    if (auto type = prop_node->first_attribute("type"))
    {
        auto type_name = std::string(type->value());

        if (type_name == "bool")
        {
            if (val == "true")
            {
                return std::make_pair(name, tpp::CustomProperty { true });
            }
            else
            {
                return std::make_pair(name, tpp::CustomProperty { false });
            }
        }
        else if (type_name == "color")
        {
            std::string_view view = val;
            view.remove_prefix(1);

            auto hex = tpp::detail::parseHex(view).value_or(0xff000000);

            tpp::Pixel colour { hex };
            return std::make_pair(name, tpp::CustomProperty { colour });
        }
        else if (type_name == "float")
        {
            return std::make_pair(name, tpp::CustomProperty { tpp::detail::parseFloat(val).value_or(0.0f) });
        }
        else if (type_name == "int")
        {
            return std::make_pair(name, tpp::CustomProperty { tpp::detail::parseInt(val).value_or(0) });
        }
        else
        {
            tpp::Error err { "[Custom Property] Unknown or unimplemented attribute type found" };
            return tl::make_unexpected(err);
        }
    }
    else
    {
        return std::make_pair(name, tpp::CustomProperty { val });
    }
}

tpp::Result<tpp::PropertyMap> tpp::PropertyMap::fromNode(const rapidxml::xml_node<char>* node)
{
    PropertyMap out {};

    for (auto prop = node->first_node("property"); prop != nullptr; prop = prop->next_sibling("property"))
    {
        auto result = parseCustomProperty(prop);

        // TODO: logging or warning propagation
        if (result)
        {
            out.data.emplace(result.value());
        }
    }

    return out;
}