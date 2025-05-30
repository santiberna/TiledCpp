#include "tiledcpp/tiledcpp.hpp"
#include "tiledcpp/detail/parse_helpers.hpp"

#include <RapidXML/rapidxml.hpp>

#include <algorithm>

namespace stb
{
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
}

using namespace tpp;

// Helpers

template <typename F>
void iterateNodeName(const rapidxml::xml_node<char>* parent_node, const char* name, F&& function)
{
    for (auto child = parent_node->first_node(name); child != nullptr; child = child->next_sibling(name))
    {
        function(child);
    }
}

Result<CustomProperty> parseCustomProperty(const rapidxml::xml_node<char>* prop_node)
{
    auto* name_attrib = prop_node->first_attribute("name");
    auto* val_attrib = prop_node->first_attribute("value");

    if (name_attrib == nullptr || val_attrib == nullptr)
    {
        Error err { "[Custom Property] could not find name or value attributes" };
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
                return CustomProperty { true };
            }
            else
            {
                return CustomProperty { false };
            }
        }
        else if (type_name == "color")
        {
            std::string_view view = val;
            view.remove_prefix(1);

            auto hex = detail::parseHex(view).value_or(0xff000000);

            Pixel colour {};
            colour.a = hex >> 24 & 0xff;
            colour.r = hex >> 16 & 0xff;
            colour.g = hex >> 8 & 0xff;
            colour.b = hex >> 0 & 0xff;

            return CustomProperty { colour };
        }
        else if (type_name == "float")
        {
            return CustomProperty { detail::parseFloat(val).value_or(0.0f) };
        }
        else if (type_name == "int")
        {
            return CustomProperty { detail::parseInt(val).value_or(0) };
        }
        else
        {
            Error err { "[Custom Property] Unknown or unimplemented attribute type found" };
            return tl::make_unexpected(err);
        }
    }
    else
    {
        return CustomProperty { val };
    }
}

// PropertyMap parsePropertyMap(const rapidxml::xml_node<char>& node)
// {
//     auto processIntoVariant = []() -> CustomProperty {

//     }
// }

// Implementation

Result<Image> Image::fromPath(const std::string& path)
{
    int width, channels, height;
    auto* data = stb::stbi_load(path.c_str(), &width, &height, &channels, 4);

    if (data == nullptr)
    {
        Error err { std::string("[Image] Failed to load image: ") + path };
        return tl::make_unexpected(err);
    }

    Image out {};
    out.size = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

    out.data = std::shared_ptr<Pixel>((Pixel*)data, [](void* ptr)
        { stb::stbi_image_free(ptr); });

    return out;
}

Pixel Image::getPixel(const UVec2& pos) const
{
    return data.get()[pos.x + pos.y * size.x];
}

void Image::freeData()
{
    data.reset();
}

Result<TileSet> TileSet::fromTSX(const std::string& path)
{
    TileSet out {};

    auto base_path = detail::getDirectory(path);
    auto extension = detail::getExtension(path);

    if (extension != ".tsx" && extension != ".xml")
    {
        Error err { std::string("[Tileset] is not a .tsx or .xml file: ") + path };
        return tl::make_unexpected(err);
    }

    std::string text = detail::getFileContents(path);
    if (text.empty())
    {
        Error err { std::string("[Tileset] file not found or unable to open: ") + path };
        return tl::make_unexpected(err);
    }

    rapidxml::xml_document<char> document {};
    document.parse<0>(text.data());

    std::string source_image_path {};

    if (auto set_node = document.first_node("tileset"))
    {
        out.tile_size.x = detail::parseInt(set_node->first_attribute("tilewidth")->value()).value();
        out.tile_size.y = detail::parseInt(set_node->first_attribute("tileheight")->value()).value();
        out.tile_count = detail::parseInt(set_node->first_attribute("tilecount")->value()).value();
        out.tile_stride = detail::parseInt(set_node->first_attribute("columns")->value()).value();

        if (auto margin = set_node->first_attribute("margin"))
        {
            out.margin = detail::parseInt(margin->value()).value();
        }

        if (auto spacing = set_node->first_attribute("spacing"))
        {
            out.spacing = detail::parseInt(spacing->value()).value();
        }

        out.name = set_node->first_attribute("name")->value();
        source_image_path = set_node->first_node("image")->first_attribute("source")->value();

        // Iterate all tile properties

        for (auto tile_node = set_node->first_node("tile"); tile_node != nullptr; tile_node = tile_node->next_sibling("tile"))
        {
            TileMetadata tile_data {};
            auto tile_id = detail::parseInt(tile_node->first_attribute("id")->value()).value();

            if (auto properties = tile_node->first_node("properties"))
            {
                PropertyMap map;
                for (auto prop_node = properties->first_node("property"); prop_node != nullptr; prop_node = prop_node->next_sibling("property"))
                {
                    std::string name = prop_node->first_attribute("name")->value();
                    std::string val = prop_node->first_attribute("value")->value();

                    if (auto type = prop_node->first_attribute("type"))
                    {
                        auto type_name = std::string(type->value());

                        if (type_name == "bool")
                        {
                            if (val == "true")
                            {
                                map.properties.emplace(name, CustomProperty { true });
                            }
                            else
                            {
                                map.properties.emplace(name, CustomProperty { false });
                            }
                        }
                        else if (type_name == "color")
                        {
                            std::string_view view = val;
                            view.remove_prefix(1);

                            auto hex = detail::parseHex(view).value_or(0xff000000);

                            Pixel colour {};
                            colour.a = hex >> 24 & 0xff;
                            colour.r = hex >> 16 & 0xff;
                            colour.g = hex >> 8 & 0xff;
                            colour.b = hex >> 0 & 0xff;

                            map.properties.emplace(name, CustomProperty { colour });
                        }
                        else if (type_name == "float")
                        {
                            map.properties.emplace(name, CustomProperty { detail::parseFloat(val).value_or(0.0f) });
                        }
                        else if (type_name == "int")
                        {
                            map.properties.emplace(name, CustomProperty { detail::parseInt(val).value_or(0) });
                        }
                        else
                        {
                            // map.emplace(name, CustomProperty { std::monostate() });
                        }
                    }
                    else
                    {
                        map.properties.emplace(name, CustomProperty { val });
                    }
                }

                tile_data.property_map = std::make_unique<PropertyMap>(std::move(map));
            }

            // Animations
            if (auto animation_info = tile_node->first_node("animation"))
            {
                Animation animation {};

                auto frame_node = animation_info->first_node("frame");
                while (frame_node != nullptr)
                {
                    KeyFrame frame {};

                    frame.tile_id = detail::parseInt(frame_node->first_attribute("tileid")->value()).value();
                    frame.duration_ms = detail::parseInt(frame_node->first_attribute("duration")->value()).value();

                    animation.frames.emplace_back(frame);
                    frame_node = frame_node->next_sibling("frame");
                }

                tile_data.animation = animation;
            }

            out.metadata.emplace(tile_id, std::move(tile_data));
        }
    }

    auto image_path = detail::resolvePath(base_path, source_image_path);
    auto image_result = Image::fromPath(image_path);

    if (image_result)
    {
        out.image = std::move(image_result.value());
    }
    else
    {
        return tl::make_unexpected(image_result.error());
    }

    return out;
}

std::optional<URect> TileSet::getTileRect(uint32_t index) const
{
    if (index >= tile_count)
        return std::nullopt;

    uint32_t column = index % tile_stride;
    uint32_t row = index / tile_stride;

    URect out {};
    out.size = tile_size;

    out.start.x = column * (tile_size.x + spacing) + margin;
    out.start.y = row * (tile_size.y + spacing) + margin;

    return out;
}

const TileMetadata* TileSet::getTileMetadata(uint32_t id) const
{
    if (auto it = metadata.find(id); it != metadata.end())
    {
        return &it->second;
    }
    return nullptr;
}

Result<TileMap> TileMap::fromTMX(const std::string& path)
{
    auto base = detail::getDirectory(path);
    auto extension = detail::getExtension(path);

    if (extension != ".tmx" && extension != ".xml")
    {
        Error err { std::string("[Tilemap] Not .tmx or .xml file: ") + path };
        return tl::make_unexpected(err);
    }

    std::string text = detail::getFileContents(path);
    if (text.empty())
    {
        Error err { std::string("[Tileset] file not found or unable to open: ") + path };
        return tl::make_unexpected(err);
    }

    rapidxml::xml_document<char> document {};
    document.parse<0>(text.data());

    TileMap out {};

    if (auto map_node = document.first_node("map"))
    {
        // Parse map data

        out.map_size.x = detail::parseInt(map_node->first_attribute("width")->value()).value();
        out.map_size.y = detail::parseInt(map_node->first_attribute("height")->value()).value();
        out.map_tile_size.x = detail::parseInt(map_node->first_attribute("tilewidth")->value()).value();
        out.map_tile_size.y = detail::parseInt(map_node->first_attribute("tileheight")->value()).value();

        // Parse all tilesets

        std::vector<uint32_t> first_gids {};
        for (auto tileset = map_node->first_node("tileset"); tileset != nullptr; tileset = tileset->next_sibling("tileset"))
        {
            std::string path = tileset->first_attribute("source")->value();
            first_gids.emplace_back(detail::parseInt(tileset->first_attribute("firstgid")->value()).value());

            auto tileset_result = TileSet::fromTSX(detail::resolvePath(base, path));

            if (tileset)
            {
                out.tile_sets.emplace_back(std::move(tileset_result.value()));
            }
            else
            {
                Error e { "[Tilemap] Error Loading Tileset\n" + tileset_result.error().message };
                return tl::make_unexpected(e);
            }
        }

        std::sort(first_gids.begin(), first_gids.end());

        // Parse all layer data

        for (auto layer = map_node->first_node("layer"); layer != nullptr; layer = layer->next_sibling("layer"))
        {
            std::string_view csv_data = { layer->first_node("data")->value(), layer->first_node("data")->value_size() };

            auto tile_indices = detail::parseAllInts(csv_data);

            TileLayer mapped_layer {};
            mapped_layer.tile_ids.reserve(tile_indices.size());

            for (auto tile_id : tile_indices)
            {
                for (uint32_t rev_it = first_gids.size() - 1; rev_it < first_gids.size(); rev_it--)
                {
                    auto first = first_gids.at(rev_it);

                    if ((uint32_t)tile_id >= first)
                    {
                        mapped_layer.tile_ids.emplace_back(TileID { rev_it, tile_id - first });
                        break;
                    }
                }
            }

            out.tile_layers.emplace_back(mapped_layer);
        }
    }

    return out;
}
