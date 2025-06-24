# TiledCpp: C++17 Parser for Tiled maps

TiledCpp is a C++ parser for the `.tmx` and `.tsx` Tiled formats. It directly loads a map, all of its layers, attributes and tilesets so that you can use them in your 2D engine.

## Quick Start

```c++
// include tilecpp.hpp
#include <tiledcpp/tiledcpp.hpp>
```

The library uses static methods to initialize objects, returning a `expected` that you can handle gracefully or `.value()` to get:

```c++
tpp::Result level_map = tpp::TileMap::fromTMX("assets/my_map.tmx").value();
tpp::TileSet unit_spritesheet = tpp::TileSet::fromTSX("assets/my_spritesheet.tsx").value();
```

Image data is automatically loaded into memory when a TileSet is loaded:

```c++
std::vector<GPUImage> tileset_images{};
auto map = tpp::TileMap::fromTMX("assets/my_map.tmx").value();

for (tpp::TileSet& tileset : map.getTileSets())
{
    tpp::Image image = tileset.getImage();
    tileset_images.emplace_back(GPUImage(renderer, image.getData(), image.getSize()));
    image.freeData(); // Uploaded to GPU, no need to have it anymore
}
```

Then, just iterate through every layer and draw every tile:

```c++
for (tpp::TileLayer& layer : map.getTileLayers())
{
    tpp::UVec2 map_tile_size = level.map.getMapTileSize();
    for (auto it = layer.tile_ids.begin(); it != layer.tile_ids.end(); ++it)
    {
        auto tile_id = *it;

        if (!tile_id.isValid())
            continue;

        tpp::UVec2 coords = it.getIndices();
        uint32_t tileset_index = tile_id.getTileset();
       
        auto& texture = tileset_images.at(tile_id.getTileset());

        tpp::TileSet& tileset = map.getTileset(tileset_index);
        tpp::URect src_rect = tileset.getTileRect(tile_id).value();

        tpp::URect dst_rect {
            { (float)(coords.x * map_tile_size.x), (float)(coords.y * map_tile_size.y) },
            { (float)(map_tile_size.x), (float)(map_tile_size.y) }
        };

        renderer.RenderTextureRect(texture, dst_rect, src_rect);
    }
}
```

## Using the library - CMake

The preferred method is through using CMake's ``FetchContent``:

```c++
include(FetchContent)

FetchContent_Declare(
    tiledcpp
    GIT_REPOSITORY https://github.com/santiberna/TiledCPP.git
    GIT_TAG v1.0.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(tiledcpp)
```

Otherwise, you can always clone the repository and the subdirectory:

```
add_subdirectory(Folder/To/TiledCpp)
```

## Samples and Demos

Check out the samples repository for more advanced and detailed examples:

[Link](https://github.com/santiberna/TiledCppSamples)

## Supported Features

- [x] Maps
  - [x] Orthogonal Maps
  - [ ] Isometric Maps
  - [ ] Hexagonal Maps
  - [ ] Parallax
  - [ ] Infinite Maps
- [x] Tilesets
  - [x] Spacing and Margins
  - [x] External Images
  - [ ] Object alignment
  - [ ] Fill Mode
  - [ ] Wangsets
  - [ ] Embedded Images
- [x] Layers
  - [x] Tile Layers
  - [ ] Groups
  - [ ] Object Groups
  - [ ] Objects
  - [ ] Image Layers
  - [ ] Text
  - [ ] Polygons
  - [ ] Compressed Tile Data  
- [x] Animations
- [x] Custom Properties
- [ ] Templates
