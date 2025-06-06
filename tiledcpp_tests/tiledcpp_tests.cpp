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
    EXPECT_EQ(result->getTileCount(), 81);
    EXPECT_EQ(result->getTileSize(), (tpp::UVec2 { 2, 2 }));
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

    auto meta = result->getTileMetadata(0);
    ASSERT_TRUE(meta != nullptr);
    ASSERT_TRUE(meta->animation.has_value());

    auto& frames = meta->animation->frames;
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

    auto meta = result->getTileMetadata(1);

    ASSERT_TRUE(meta != nullptr);
    ASSERT_TRUE(meta->custom_properties != nullptr);
    ASSERT_TRUE(meta->custom_properties->data.size() > 0);

    auto& map = *meta->custom_properties;
    auto checkProp = [](tpp::CustomProperty prop, auto expected_val)
    {
        auto* val = std::get_if<decltype(expected_val)>(&prop.value);

        ASSERT_TRUE(val != nullptr);
        EXPECT_EQ(*val, expected_val);
    };

    checkProp(map.data["BoolProp"], true);
    checkProp(map.data["IntProp"], 42);
    checkProp(map.data["FloatProp"], 3.14f);
    checkProp(map.data["StringProp"], std::string("Hello World"));
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

    ASSERT_EQ(layer1.tile_ids.size(), 8);
    ASSERT_EQ(layer2.tile_ids.size(), 8);

    EXPECT_EQ(layer1.tile_ids.at(3).tileset, 0);
    EXPECT_EQ(layer1.tile_ids.at(3).id, 3);

    for (auto tile : layer2.tile_ids)
    {
        EXPECT_EQ(tile.tileset, 1);
        EXPECT_EQ(tile.id, 1);
    }
}

TEST(TileMapTests, MapProperties)
{
    auto result = tpp::TileMap::fromTMX("tiledcpp_tests/files/map1.tmx");
    ASSERT_TRUE(result.has_value()) << result.error().message;
    ASSERT_EQ(result->getTileLayers().size(), 2);

    auto& layer = result->getTileLayers().at(0);

    ASSERT_TRUE(result.value().getProperties() != nullptr);
    auto val = result->getProperties()->data.at("TestProperty");
    EXPECT_TRUE(std::get_if<float>(&val.value) != nullptr);

    ASSERT_TRUE(layer.custom_properties != nullptr);
    auto val2 = layer.custom_properties->data.at("TestProperty");
    EXPECT_TRUE(std::get_if<float>(&val2.value) != nullptr);
}