#pragma once

#include <concepts>
#include <type_traits>

template <typename Comp>
concept EmptyComponent = std::is_empty_v<Comp> && std::is_default_constructible_v<Comp>;


template <typename Comp>
concept NotEmptyComponent = !std::is_empty_v<Comp> || !std::is_default_constructible_v<Comp>;
