#include <tiledcpp/types/math.hpp>
#include <vector>

namespace tpp
{

template <typename T>
class Array2DIterator
{
private:
    static constexpr bool IS_CONST = std::is_const_v<T>;

    using Self = Array2DIterator<T>;
    using Value = std::remove_const_t<T>;

    using UnderlyingIterator = std::conditional_t<
        IS_CONST,
        typename std::vector<Value>::const_iterator,
        typename std::vector<Value>::iterator>;

public:
    Array2DIterator() = default;
    Array2DIterator(UnderlyingIterator base, UnderlyingIterator iterator, uint32_t stride);

    T& operator*();
    T* operator->();
    Self& operator++();
    Self& operator++(int);
    bool operator==(const Self& other) const;
    bool operator!=(const Self& other) const;

    tpp::UVec2 getIndices() const;

private:
    UnderlyingIterator base {};
    UnderlyingIterator iterator {};
    uint32_t stride {};
};

// Fixed size 2D Array
template <typename T>
class Array2D
{
public:
    Array2D() = default;
    Array2D(uint32_t width, uint32_t height);
    Array2D(uint32_t width, uint32_t height, const T& init);

    T& at(uint32_t i, uint32_t j);
    const T& at(uint32_t i, uint32_t j) const;

    T& at(const UVec2& ij) { return at(ij.x, ij.y); }
    const T& at(const UVec2 ij) const { return at(ij.x, ij.y); }

    UVec2 size() const { return array_size; }

    Array2DIterator<T> begin();
    Array2DIterator<T> end();

    Array2DIterator<const T> begin() const;
    Array2DIterator<const T> end() const;

private:
    UVec2 array_size {};
    std::vector<T> data;
};

template <typename T>
Array2DIterator<T>::Array2DIterator(UnderlyingIterator base, UnderlyingIterator iterator, uint32_t stride)
    : base(base)
    , iterator(iterator)
    , stride(stride)
{
}

template <typename T>
T& Array2DIterator<T>::operator*()
{
    return *iterator;
}

template <typename T>
T* Array2DIterator<T>::operator->()
{
    return &(*iterator);
}

template <typename T>
Array2DIterator<T>& Array2DIterator<T>::operator++()
{
    ++iterator;
    return *this;
}

template <typename T>
Array2DIterator<T>& Array2DIterator<T>::operator++(int)
{
    Self temp = *this;
    ++(*this);
    return temp;
}

template <typename T>
bool Array2DIterator<T>::operator==(const Self& other) const
{
    return iterator == other.iterator;
}

template <typename T>
bool Array2DIterator<T>::operator!=(const Self& other) const
{
    return !(*this == other);
}

template <typename T>
UVec2 Array2DIterator<T>::getIndices() const
{
    auto offset = std::distance(base, iterator);
    return UVec2 {
        static_cast<uint32_t>(offset % stride),
        static_cast<uint32_t>(offset / stride)
    };
}

template <typename T>
Array2D<T>::Array2D(uint32_t width, uint32_t height)
    : array_size(width, height)
    , data(width * height)
{
}

template <typename T>
Array2D<T>::Array2D(uint32_t width, uint32_t height, const T& init)
    : array_size(width, height)
    , data(width * height, init)
{
}

template <typename T>
T& Array2D<T>::at(uint32_t x, uint32_t y)
{
    return data[y * array_size.x + x];
}

template <typename T>
const T& Array2D<T>::at(uint32_t x, uint32_t y) const
{
    return data[y * array_size.x + x];
}

template <typename T>
Array2DIterator<T> Array2D<T>::begin()
{
    Array2DIterator<T> it(data.begin(), data.begin(), array_size.x);
    return it;
}

template <typename T>
Array2DIterator<T> Array2D<T>::end()
{
    Array2DIterator<T> it(data.begin(), data.end(), array_size.x);
    return it;
}

template <typename T>
Array2DIterator<const T> Array2D<T>::begin() const
{
    Array2DIterator<const T> it(data.begin(), data.begin(), array_size.x);
    return it;
}

template <typename T>
Array2DIterator<const T> Array2D<T>::end() const
{
    Array2DIterator<const T> it(data.begin(), data.end(), array_size.x);
    return it;
}

}