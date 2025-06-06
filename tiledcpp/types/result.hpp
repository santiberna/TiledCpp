#pragma once
#include "tiledcpp/config.hpp"

#include <expected/expected.hpp>
#include <string>

namespace tpp
{

struct TILEDCPP_API Error
{
    std::string message {};
};

template <typename T>
using Result = tl::expected<T, Error>;

}