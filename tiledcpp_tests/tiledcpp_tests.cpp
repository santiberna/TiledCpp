#include <gtest/gtest.h>
#include <tiledcpp/tiledcpp.hpp>

TEST(ImageTests, LoadImageFile)
{
    auto result = tpp::Image::fromPath("tiledcpp_tests/files/image.png");

    ASSERT_TRUE(result.has_value()) << result.error().message;
    EXPECT_EQ(result->getSize(), (tpp::UVec2 { 18, 18 }));
}

TEST(TileSetTests, LoadBasicTileset)
{
    auto result = tpp::TileSet::fromTSX("tiledcpp_tests/files/tileset.tsx");
    ASSERT_TRUE(result.has_value()) << result.error().message;

    EXPECT_EQ(result->getName(), "image");
    EXPECT_EQ(result->getTileCount(), 16);
    EXPECT_EQ(result->getTileSize(), (tpp::UVec2 { 4, 4 }));
}

TEST(TileSetTests, CheckSpacingMarginMetrics)
{
    auto result = tpp::TileSet::fromTSX("tiledcpp_tests/files/tileset2.tsx");
    ASSERT_TRUE(result.has_value()) << result.error().message;

    ASSERT_EQ(result->getTileCount(), 16);
    EXPECT_EQ(result->getTileSize(), (tpp::UVec2 { 2, 2 }));

    const auto& image = result->getImage();
    for (uint32_t i = 0; i < 16; i++)
    {
        auto image_rect = result->getTileRect(i);
        ASSERT_TRUE(image_rect.has_value());

        for (uint32_t j = image_rect->start.y; j < image_rect->start.y + image_rect->size.y; j++)
        {
            for (uint32_t k = image_rect->start.y; k < image_rect->start.y + image_rect->size.y; k++)
            {
                EXPECT_EQ(image.getPixel({ k, j }), (tpp::Pixel { 0, 255, 0, 255 }));
            }
        }
    }
}

TEST(TileSetTests, Animations)
{
    auto result = tpp::TileSet::fromTSX("tiledcpp_tests/files/tileset3.tsx");
    ASSERT_TRUE(result.has_value()) << result.error().message;
    ASSERT_EQ(result->getTileCount(), 16);

    auto anim = result->getTileAnimation(0);
    ASSERT_TRUE(anim != nullptr);

    auto& frames = anim->frames;
    ASSERT_EQ(frames.size(), 2);

    EXPECT_EQ(frames[0].duration_ms, 10);
    EXPECT_EQ(frames[1].duration_ms, 20);
    EXPECT_EQ(frames[0].tile_id, 0);
    EXPECT_EQ(frames[1].tile_id, 1);
}

TEST(TileSetTests, CustomProperties)
{
    auto result = tpp::TileSet::fromTSX("tiledcpp_tests/files/tileset3.tsx");
    ASSERT_TRUE(result.has_value()) << result.error().message;
    ASSERT_EQ(result->getTileCount(), 16);

    auto props = result->getTileProperties(1);

    ASSERT_TRUE(props != nullptr);
    ASSERT_TRUE(props != nullptr);
    ASSERT_TRUE(props->size() > 0);

    auto& map = *props;

    auto checkProp = [](const tpp::PropertyMap& map, const std::string& key, auto expected_val)
    {
        using Type = decltype(expected_val);
        ASSERT_TRUE(map.has<Type>(key));
        EXPECT_EQ(map.get<Type>(key).value(), expected_val);
    };

    checkProp(map, "BoolProp", true);
    checkProp(map, "IntProp", 42);
    checkProp(map, "FloatProp", 3.14f);
    checkProp(map, "StringProp", std::string("Hello World"));
}

TEST(TileMapTests, MapWith2Layers)
{
    auto result = tpp::TileMap::fromTMX("tiledcpp_tests/files/map1.tmx");
    ASSERT_TRUE(result.has_value()) << result.error().message;

    EXPECT_EQ(result->getTileSets().size(), 2);
    EXPECT_EQ(result->getMapTileSize(), (tpp::UVec2 { 4, 4 }));
    EXPECT_EQ(result->getMapGridSize(), (tpp::UVec2 { 4, 2 }));

    ASSERT_EQ(result->getTileLayers().size(), 2);

    auto& layer1 = result->getTileLayers().at(0);
    auto& layer2 = result->getTileLayers().at(1);

    tpp::UVec2 expected { 4, 2 };
    ASSERT_EQ(layer1.tile_ids.size(), expected);

    EXPECT_EQ(layer1.tile_ids.at(3, 0).getTileset(), 0);
    EXPECT_EQ(layer1.tile_ids.at(3, 0).getId(), 3);

    for (auto tile : layer2.tile_ids)
    {
        EXPECT_EQ(tile.getTileset(), 1);
        EXPECT_EQ(tile.getId(), 1);
    }
}

TEST(TileMapTests, MapProperties)
{
    auto result = tpp::TileMap::fromTMX("tiledcpp_tests/files/map1.tmx");
    ASSERT_TRUE(result.has_value()) << result.error().message;
    ASSERT_EQ(result->getTileLayers().size(), 2);

    auto& layer = result->getTileLayers().at(0);

    ASSERT_TRUE(result.value().getProperties() != nullptr);
    auto val = result->getProperties()->get<float>("TestProperty");
    EXPECT_TRUE(val.has_value());

    ASSERT_TRUE(layer.custom_properties != nullptr);
    auto val2 = layer.custom_properties->get<float>("TestProperty");
    EXPECT_TRUE(val2.has_value());
}

TEST(TileMapTests, FindLayer)
{
    auto result = tpp::TileMap::fromTMX("tiledcpp_tests/files/map1.tmx");
    ASSERT_TRUE(result.has_value()) << result.error().message;

    auto* found = result->findTileLayer("NamedLayer");
    EXPECT_TRUE(found != nullptr);
}

#include <bitset>

TEST(TileMapTests, FlippedTiles)
{
    auto result = tpp::TileMap::fromTMX("tiledcpp_tests/files/map2.tmx");
    ASSERT_TRUE(result.has_value()) << result.error().message;

    auto& layer = result->getTileLayers().at(0);

    for (auto& tile : layer.tile_ids)
    {
        std::cout << std::bitset<32>(tile.getId()) << std::endl;
        // EXPECT_EQ(tile.getId(), 0);
    }

    // Check flipped tiles
    EXPECT_TRUE(layer.tile_ids.at(1, 0).isFlippedHorizontally());
    EXPECT_TRUE(layer.tile_ids.at(0, 1).isFlippedVertically());
    EXPECT_TRUE(layer.tile_ids.at(1, 1).isFlippedHorizontally() && layer.tile_ids.at(1, 1).isFlippedVertically());
}

TEST(TileMapTests, EmptyTiles)
{
    auto result = tpp::TileMap::fromTMX("tiledcpp_tests/files/map3.tmx");
    ASSERT_TRUE(result.has_value()) << result.error().message;

    for (auto& tiles : result->getTileLayers().at(0).tile_ids)
    {
        EXPECT_FALSE(tiles.isValid());
    }
}