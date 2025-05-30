#include <gtest/gtest.h>
#include <tiledcpp/detail/parse_helpers.hpp>

TEST(ParseHelpersTests, ParseNumberWithOffset)
{
    std::string_view str = "1234";
    auto result = tpp::detail::parseInt(str);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1234);
}

TEST(ParseHelpersTests, ParseNumberWithOffsetNegative)
{
    std::string_view str = "-1234";
    auto result = tpp::detail::parseInt(str);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), -1234);
}

TEST(ParseHelpersTests, ParseAllNumbers)
{
    std::string_view str = "1234 -5678 90";
    auto result = tpp::detail::parseAllInts(str);

    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], 1234);
    EXPECT_EQ(result[1], -5678);
    EXPECT_EQ(result[2], 90);
}

TEST(ParseHelpersTests, ParseNumberFloat)
{
    std::string_view str = "1234.56";
    auto result = tpp::detail::parseFloat(str);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1234.56f);
}

TEST(ParseHelpersTests, ParseNumberFloatNegative)
{
    std::string_view str = "-1234.56";
    auto result = tpp::detail::parseFloat(str);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), -1234.56f);
}

TEST(ParseHelpersTests, ParseNumberInvalid)
{
    std::string_view str = "abc";
    auto result = tpp::detail::parseInt(str);

    EXPECT_FALSE(result.has_value());
}

TEST(ParseHelpersTests, ParseHex)
{
    std::string_view str = "ffffffff";
    auto result = tpp::detail::parseHex(str);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 0xffffffff);
}

TEST(ParseHelperTests, ResolvePath)
{
    std::string base = "/base/path";
    std::string relative_path = "../relative/image.png";
    std::string absolute_path = "/absolute/path/image.png";

    EXPECT_EQ(tpp::detail::resolvePath(base, relative_path), tpp::detail::makePreferredPath("/base/relative/image.png"));
    EXPECT_EQ(tpp::detail::resolvePath(base, absolute_path), tpp::detail::makePreferredPath(absolute_path));
}

TEST(ParseHelperTests, GetDirectory)
{
    std::string path = "/base/path/image.png";
    EXPECT_EQ(tpp::detail::getDirectory(path), "/base/path");
}