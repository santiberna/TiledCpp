#include "tiledcpp/tiledcpp.hpp"
#include "tiledcpp/detail/parse_helpers.hpp"

#include <RapidXML/rapidxml.hpp>

namespace stb
{
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
}

using namespace tpp;

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
        out.tile_size.x = std::stoul(set_node->first_attribute("tilewidth")->value());
        out.tile_size.y = std::stoul(set_node->first_attribute("tileheight")->value());
        out.tile_count = std::stoul(set_node->first_attribute("tilecount")->value());
        out.tile_stride = std::stoul(set_node->first_attribute("columns")->value());

        if (auto margin = set_node->first_attribute("margin"))
        {
            out.margin = std::stoul(margin->value());
        }

        if (auto spacing = set_node->first_attribute("spacing"))
        {
            out.spacing = std::stoul(spacing->value());
        }

        out.name = set_node->first_attribute("name")->value();
        source_image_path = set_node->first_node("image")->first_attribute("source")->value();

        // Iterate all tile properties
        auto tile_node = set_node->first_node("tile");
        while (tile_node != nullptr)
        {
            TileMetadata tile_data {};
            auto tile_id = std::stoul(tile_node->first_attribute("id")->value());
            
            //     std::unordered_map<std::string, std::string> properties;

            // if (auto props = tile_node->first_node("properties"))
            // {
            //     auto prop_node = props->first_node("property");

            //     while (prop_node != nullptr)
            //     {
            //         std::string name = prop_node->first_attribute("name")->value();
            //         std::string val = prop_node->first_attribute("value")->value();

            //         properties.emplace(name, val);
            //         prop_node = prop_node->next_sibling("property");
            //     }
            // }

            if (auto animation_info = tile_node->first_node("animation"))
            {
                Animation animation {};

                auto frame_node = animation_info->first_node("frame");
                while (frame_node != nullptr)
                {
                    KeyFrame frame {};

                    frame.tile_id = std::stoul(frame_node->first_attribute("tileid")->value());
                    frame.duration_ms = std::stoul(frame_node->first_attribute("duration")->value());

                    animation.frames.emplace_back(frame);
                    frame_node = frame_node->next_sibling("frame");
                }

                tile_data.animation = animation;
            }

            out.metadata.emplace(tile_id, std::move(tile_data));
            tile_node = tile_node->next_sibling("tile");
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

// Result<TileMap> TileMap::FromTMX(Renderer& renderer, const std::string& path)
// {
//     auto base = fileIO::GetDirectory(path);
//     auto extension = fileIO::GetExtension(path);

//     if (extension != ".tmx" && extension != ".xml")
//     {
//         return Failed { std::format("Tilemap: Not .tmx or .xml file: {}", path) };
//     }

//     std::string text {};
//     if (auto file = fileIO::OpenReadStream(path))
//     {
//         text = fileIO::DumpStreamIntoString(file.value());
//     }
//     else
//     {
//         return Failed { std::format("Tileset: TMX File not found: {}", path) };
//     }

//     cereal::rapidxml::xml_document<char> document {};
//     document.parse<0>(text.data());

//     TileMap out {};

//     if (auto map_node = document.first_node("map"))
//     {
//         auto tile_set_path = map_node->first_node("tileset")->first_attribute("source")->value();
//         if (auto tileset = TileSet::FromTSX(renderer, fileIO::PathAppend(base, tile_set_path)))
//         {
//             out.tiles = tileset.value();
//         }
//         else
//         {
//             return Failed { tileset.error() };
//         }

//         auto map_size_x = std::stoul(map_node->first_attribute("width")->value());
//         auto map_size_y = std::stoul(map_node->first_attribute("height")->value());
//         out.tile_size.x = std::stoul(map_node->first_attribute("tilewidth")->value());
//         out.tile_size.y = std::stoul(map_node->first_attribute("tileheight")->value());

//         auto layer_data = map_node->first_node("layer");
//         std::string_view view = { layer_data->first_node("data")->value(), layer_data->first_node("data")->value_size() };
//         auto ids = parse::AllNumbers<uint32_t>(view);

//         out.tile_ids = Array2D<uint32_t>(std::move(ids), map_size_x, map_size_y);
//     }

//     return out;
// }
