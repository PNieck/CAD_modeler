#pragma once

#include "system.hpp"

#include <concepts>
#include <type_traits>


template<typename Sys>
concept SystemConcept = std::derived_from<Sys, System> && std::is_default_constructible_v<Sys>;
