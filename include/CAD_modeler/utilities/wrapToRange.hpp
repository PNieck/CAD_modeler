#pragma once

#include <concepts>
#include <cmath>

template <std::floating_point T>
constexpr T WrapToRange(T v, T max)
{
    return std::fmod( std::fmod(v, max) + max, max);
}
