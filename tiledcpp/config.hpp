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