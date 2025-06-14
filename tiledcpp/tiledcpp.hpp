#pragma once

#include "tiledcpp/config.hpp"

#include <optional>

#include "tiledcpp/types/array2d.hpp"
#include "tiledcpp/types/image.hpp"
#include "tiledcpp/types/properties.hpp"

namespace tpp
{

struct TILEDCPP_API KeyFrame
{
    uint32_t tile_id {};
    uint32_t duration_ms {};
};

struct TILEDCPP_API Animation
{
    std::vector<KeyFrame> frames {};
};

class TILEDCPP_API TileSet
{
public:
    TileSet() = default;

    // Non copyable
    TileSet(const TileSet&) = delete;
    TileSet& operator=(const TileSet&) = delete;

    // Movable
    TileSet(TileSet&&) = default;
    TileSet& operator=(TileSet&&) = default;

    static Result<TileSet> fromTSX(const std::string& path, std::ostream* warnings = nullptr);

    std::optional<URect> getTileRect(uint32_t tile_id) const;
    uint32_t getTileCount() const { return tile_count; }
    UVec2 getTileSize() const { return tile_size; }

    Image& getImage() { return image; }
    const Image& getImage() const { return image; }

    const std::string& getName() const { return name; }
    const PropertyMap* getProperties() const { return custom_properties.get(); }
    const PropertyMap* getTileProperties(uint32_t tile) const;
    const Animation* getTileAnimation(uint32_t tile) const;

private:
    std::unique_ptr<PropertyMap> custom_properties {};

    std::unordered_map<uint32_t, PropertyMap> tile_properties {};
    std::unordered_map<uint32_t, Animation> tile_animations {};

    std::string name {};
    Image image {};

    uint32_t tile_count {};
    uint32_t tile_stride {};

    uint32_t margin {};
    uint32_t spacing {};

    UVec2 tile_size {};
};

struct TILEDCPP_API TileID
{
    uint32_t tileset {};
    uint32_t id {};
};

struct TILEDCPP_API TileLayer
{
    std::string name {};
    Array2D<TileID> tile_ids;
    std::unique_ptr<PropertyMap> custom_properties {};
};

class TILEDCPP_API TileMap
{
public:
    TileMap() = default;

    // Non copyable
    TileMap(const TileMap&) = delete;
    TileMap& operator=(const TileMap&) = delete;

    // Movable
    TileMap(TileMap&&) = default;
    TileMap& operator=(TileMap&&) = default;

    static Result<TileMap> fromTMX(const std::string& path, std::ostream* warnings = nullptr);

    std::vector<TileSet>& getTileSets() { return tile_sets; }
    std::vector<TileLayer>& getTileLayers() { return tile_layers; }
    const std::vector<TileSet>& getTileSets() const { return tile_sets; }
    const std::vector<TileLayer>& getTileLayers() const { return tile_layers; }

    const TileLayer* findTileLayer(const std::string& name) const;
    TileLayer* findTileLayer(const std::string& name);

    const PropertyMap* getProperties() const { return custom_properties.get(); }

    UVec2 getMapGridSize() const { return map_size; }
    UVec2 getMapTileSize() const { return map_tile_size; }

private:
    std::vector<TileSet> tile_sets {};
    std::vector<TileLayer> tile_layers {};

    UVec2 map_size {};
    UVec2 map_tile_size {};

    std::unique_ptr<PropertyMap> custom_properties {};
};
}