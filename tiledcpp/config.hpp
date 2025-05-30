#pragma once

#if defined TILEDCPP_STATIC
#define TILEDCPP_API
#else

#if defined _WIN32

#if defined TILEDCPP_EXPORT
#define TILEDCPP_API __declspec(dllexport)
#else
#define TILEDCPP_API __declspec(dllimport)
#endif // TILEDCPP_EXPORT

#endif // _WIN32

#endif // TILEDCPP_STATIC