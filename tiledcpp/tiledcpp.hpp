#pragma once

#ifdef _WIN32
#ifdef TILEDCPP_EXPORT
#define TILEDCPP_API __declspec(dllexport)
#else
#define TILEDCPP_API __declspec(dllimport)
#endif
#else
#define TILEDCPP_API
#endif

#include <expected/expected.hpp>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace tpp
{

struct Error
{
    std::string message {};
};

template <typename T>
using Result = tl::expected<T, Error>;

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
    uint8_t r {}, g {}, b {}, a {};
    bool operator==(const Pixel& o) const { return r == o.r && g == o.g && b == o.b && a == o.a; }
};

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

struct TILEDCPP_API KeyFrame
{
    uint32_t tile_id {};
    uint32_t duration_ms {};
};

struct TILEDCPP_API Animation
{
    std::vector<KeyFrame> frames {};
};

struct TILEDCPP_API CustomProperty
{
    std::variant<bool, int, float, std::string, Pixel> value;
};

struct TILEDCPP_API TileMetadata
{
    std::optional<Animation> animation {};
    std::unique_ptr<std::unordered_map<std::string, CustomProperty>> properties {};
};

class TILEDCPP_API TileSet
{
public:
    TileSet() = default;
    static Result<TileSet> fromTSX(const std::string& path);

    std::optional<URect> getTileRect(uint32_t tile_id) const;
    uint32_t getTileCount() const { return tile_count; }
    UVec2 getTileSize() const { return tile_size; }
    const Image& getImage() const { return image; }
    const std::string& getName() const { return name; }
    const TileMetadata* getTileMetadata(uint32_t id) const;

private:
    std::unordered_map<uint32_t, TileMetadata> metadata {};
    std::string name {};
    Image image {};

    uint32_t tile_count {};
    uint32_t tile_stride {};

    uint32_t margin {};
    uint32_t spacing {};

    UVec2 tile_size {};
};

class TILEDCPP_API TileMap {};

}