#pragma once
#include <ranges>

namespace kissra {
template <typename T, typename TValue>
concept can_push_back = std::ranges::range<T> && requires(T rng, TValue val) { rng.push_back(val); };

template <typename T, typename TValue>
concept can_insert = std::ranges::range<T> && requires(T rng, TValue val) { rng.insert(std::ranges::end(rng), val); };

template <typename T>
concept can_reserve = std::ranges::range<T> && requires(T rng) { rng.reserve(1); };
} // namespace kissra