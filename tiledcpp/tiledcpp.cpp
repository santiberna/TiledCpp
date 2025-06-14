#include "tiledcpp/tiledcpp.hpp"

#include "tiledcpp/detail/parse_helpers.hpp"
#include "tiledcpp/detail/xml_helpers.hpp"

#include <algorithm>

using namespace tpp;

// Helpers

std::optional<int> getIntAttribute(const rapidxml::xml_node<char>* node, const char* name)
{
    if (auto attrib = node->first_attribute(name))
    {
        return detail::parseInt(attrib->value());
    }
    return std::nullopt;
}

std::unique_ptr<PropertyMap> tryGetProperties(const rapidxml::xml_node<char>* node)
{
    if (auto* props = node->first_node("properties"))
    {
        auto result = PropertyMap::fromNode(props);
        if (result)
        {
            return std::make_unique<PropertyMap>(std::move(result.value()));
        }
    }
    return nullptr;
}

// Implementation

Result<TileSet> TileSet::fromTSX(const std::string& path, [[maybe_unused]] std::ostream* warnings)
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
        out.tile_size.x = getIntAttribute(set_node, "tilewidth").value_or(0);
        out.tile_size.y = getIntAttribute(set_node, "tileheight").value_or(0);
        out.tile_count = getIntAttribute(set_node, "tilecount").value_or(0);
        out.tile_stride = getIntAttribute(set_node, "columns").value_or(0);
        out.margin = getIntAttribute(set_node, "margin").value_or(0);
        out.spacing = getIntAttribute(set_node, "spacing").value_or(0);

        out.name = set_node->first_attribute("name")->value();
        source_image_path = set_node->first_node("image")->first_attribute("source")->value();

        // Custom Props

        out.custom_properties = tryGetProperties(set_node);

        // Iterate all tile properties

        for (auto tile_node = set_node->first_node("tile"); tile_node != nullptr; tile_node = tile_node->next_sibling("tile"))
        {
            auto tile_id = detail::parseInt(tile_node->first_attribute("id")->value()).value();

            if (auto properties = tile_node->first_node("properties"))
            {
                PropertyMap map = PropertyMap::fromNode(properties).value();
                out.tile_properties.emplace(tile_id, std::move(map));
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

                out.tile_animations.emplace(tile_id, std::move(animation));
            }
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

const PropertyMap* TileSet::getTileProperties(uint32_t tile) const
{
    if (auto it = tile_properties.find(tile); it != tile_properties.end())
    {
        return &it->second;
    }
    return nullptr;
}

const Animation* TileSet::getTileAnimation(uint32_t tile) const
{
    if (auto it = tile_animations.find(tile); it != tile_animations.end())
    {
        return &it->second;
    }
    return nullptr;
}

Result<TileMap> TileMap::fromTMX(const std::string& path, [[maybe_unused]] std::ostream* warnings)
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

        out.map_size.x = getIntAttribute(map_node, "width").value_or(0);
        out.map_size.y = getIntAttribute(map_node, "height").value_or(0);
        out.map_tile_size.x = getIntAttribute(map_node, "tilewidth").value_or(0);
        out.map_tile_size.y = getIntAttribute(map_node, "tileheight").value_or(0);

        // Custom Properties

        out.custom_properties = tryGetProperties(map_node);

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
            mapped_layer.tile_ids = Array2D<TileID>(out.map_size.x, out.map_size.y);

            size_t index = 0;
            for (auto& tile_id : mapped_layer.tile_ids)
            {
                auto t = tile_indices.at(index);

                for (uint32_t rev_it = first_gids.size() - 1; rev_it < first_gids.size(); rev_it--)
                {
                    auto first = first_gids.at(rev_it);

                    if ((uint32_t)t >= first)
                    {
                        tile_id = TileID { rev_it, t - first };
                        break;
                    }
                }

                index += 1;
            }

            // Custom Properties
            mapped_layer.custom_properties = tryGetProperties(layer);
            out.tile_layers.emplace_back(std::move(mapped_layer));
        }
    }

    return out;
}
