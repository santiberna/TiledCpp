#include <gtest/gtest.h>
#include <tiledcpp/types/array2d.hpp>

TEST(Array2DTest, ConstructWithDefaultValue)
{
    tpp::Array2D<int> array(3, 2, 42);

    for (size_t y = 0; y < 2; ++y)
    {
        for (size_t x = 0; x < 3; ++x)
        {
            EXPECT_EQ(array.at(x, y), 42);
        }
    }
}

TEST(Array2DTest, ModifyAndAccessElement)
{
    tpp::Array2D<std::string> array(2, 2);
    array.at(0, 1) = "hello";
    array.at(1, 0) = "world";

    EXPECT_EQ(array.at(0, 1), "hello");
    EXPECT_EQ(array.at(1, 0), "world");
}

TEST(Array2DTest, IteratorTraversalOrder)
{
    tpp::Array2D<int> array(2, 2);
    array.at(0, 0) = 1;
    array.at(1, 0) = 2;
    array.at(0, 1) = 3;
    array.at(1, 1) = 4;

    std::vector<int> expected = { 1, 2, 3, 4 };
    std::vector<int> actual;

    for (auto it = array.begin(); it != array.end(); ++it)
    {
        actual.push_back(*it);
    }

    EXPECT_EQ(actual, expected);
}

TEST(Array2DTest, GetIndicesFromIterator)
{
    tpp::Array2D<int> array(3, 2, 0);

    for (auto it = array.begin(); it != array.end(); ++it)
    {
        auto [x, y] = it.getIndices();
        array.at(x, y) = 1;
    }

    for (size_t y = 0; y < 2; ++y)
    {
        for (size_t x = 0; x < 3; ++x)
        {
            EXPECT_EQ(array.at(x, y), 1);
        }
    }
}

TEST(Array2DTest, ConstIteratorTraversal)
{
    tpp::Array2D<int> array(2, 2, 7);
    const auto& const_ref = array;

    using v = const std::vector<int>;
    v::iterator it {};
    for (auto it = const_ref.begin(); it != const_ref.end(); ++it)
    {
        EXPECT_EQ(*it, 7);
    }
}

TEST(Array2DTest, RangeBasedForLoop)
{
    tpp::Array2D<int> array(2, 2, 7);

    for (auto& elem : array)
    {
        EXPECT_EQ(elem, 7);
        elem = 3;
    }

    const auto& const_ref = array;

    for (auto elem : const_ref)
    {
        EXPECT_EQ(elem, 3);
    }
}