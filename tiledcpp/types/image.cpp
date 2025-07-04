#include "tiledcpp/types/image.hpp"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"

tpp::Result<tpp::Image> tpp::Image::fromPath(const std::string& path)
{
    int width, channels, height;
    auto* data = stbi_load(path.c_str(), &width, &height, &channels, 4);

    if (data == nullptr)
    {
        std::string message = "[TiledCpp Image] Failed to load image (" + path + "): " + stbi_failure_reason();
        Error err { message };

        return tl::make_unexpected(err);
    }

    Image out {};
    out.size = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

    out.data = std::shared_ptr<Pixel>((Pixel*)data, [](void* ptr)
        { stbi_image_free(ptr); });

    return out;
}

tpp::Pixel tpp::Image::getPixel(const tpp::UVec2& pos) const
{
    return data.get()[pos.x + pos.y * size.x];
}

void tpp::Image::freeData()
{
    data.reset();
}