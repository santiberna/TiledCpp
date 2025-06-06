#include "tiledcpp/types/math.hpp"

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