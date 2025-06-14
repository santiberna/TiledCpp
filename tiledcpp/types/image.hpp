#pragma once
#include "tiledcpp/config.hpp"

#include <memory>

#include "tiledcpp/types/math.hpp"
#include "tiledcpp/types/result.hpp"

namespace tpp
{

class TILEDCPP_API Image
{
public:
    Image() = default;
    ~Image() = default;

    // Non copyable
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;

    // Movable
    Image(Image&&) = default;
    Image& operator=(Image&&) = default;

    static Result<Image> fromPath(const std::string& path);
    void freeData();

    Pixel getPixel(const UVec2& pos) const;
    void* getData() const { return data.get(); }
    UVec2 getSize() const { return size; }

private:
    UVec2 size {};
    std::shared_ptr<Pixel> data {};
};

}