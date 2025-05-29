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

TEST(TileSetTests, CheckAnimation)
{
    auto result = tpp::TileSet::fromTSX("tiledcpp_tests/files/tileset3.tsx");
    ASSERT_TRUE(result.has_value()) << result.error().message;
    ASSERT_EQ(result->getTileCount(), 16);

    auto animation_tile = result->getTileMetadata(0);
    ASSERT_TRUE(animation_tile != nullptr);
    ASSERT_TRUE(animation_tile->animation.has_value());

    auto& frames = animation_tile->animation->frames;
    ASSERT_EQ(frames.size(), 2);

    EXPECT_EQ(frames[0].duration_ms, 10);
    EXPECT_EQ(frames[1].duration_ms, 20);
    EXPECT_EQ(frames[0].tile_id, 0);
    EXPECT_EQ(frames[1].tile_id, 1);
}