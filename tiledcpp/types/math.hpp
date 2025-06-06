#pragma once
#include "tiledcpp/config.hpp"

#include <cstdint>

namespace tpp
{

struct TILEDCPP_API UVec2
{
    uint32_t x {};
    uint32_t y {};

    bool operator==(const UVec2& o) const { return x == o.x && y == o.y; }
};

struct TILEDCPP_API URect
{
    UVec2 start {};
    UVec2 size {};
};

struct TILEDCPP_API Pixel
{
    Pixel() = default;
    Pixel(uint8_t a, uint8_t r, uint8_t g, uint8_t b);
    Pixel(uint32_t argb);

    uint8_t a {}, r {}, g {}, b {};
    bool operator==(const Pixel& o) const { return r == o.r && g == o.g && b == o.b && a == o.a; }
};

}