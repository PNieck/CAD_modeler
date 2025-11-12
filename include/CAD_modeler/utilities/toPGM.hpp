#pragma once

#include "flatVec2D.hpp"

#include <string_view>


void ToPGM(const FlatVec2D<float>& data, std::string_view path);
