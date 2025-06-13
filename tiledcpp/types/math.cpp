#include "tiledcpp/types/math.hpp"

tpp::IVec2::IVec2(int32_t x, int32_t y)
    : x(x)
    , y(y)
{
}

tpp::IVec2::IVec2(const UVec2& uvec)
    : x(static_cast<int>(uvec.x))
    , y(static_cast<int>(uvec.y))
{
}

tpp::Pixel::Pixel(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
    : a(a)
    , r(r)
    , g(g)
    , b(b)
{
}

tpp::Pixel::Pixel(uint32_t hex)
{
    a = hex >> 24 & 0xff;
    r = hex >> 16 & 0xff;
    g = hex >> 8 & 0xff;
    b = hex >> 0 & 0xff;
}