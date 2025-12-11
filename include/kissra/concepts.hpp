#pragma once
#include <ranges>

namespace kissra {

template <typename T>
concept iterator = requires(T& t) {
    typename T::value_type;
    typename T::reference;
    typename T::const_reference;
    { T::is_sized } -> std::same_as<const bool&>;
    { T::is_common } -> std::same_as<const bool&>;
    { T::is_forward } -> std::same_as<const bool&>;
    { T::is_bidir } -> std::same_as<const bool&>;
    { T::is_random } -> std::same_as<const bool&>;
    typename T::template ref_t<T&>;
    typename T::template ref_t<const T&>;
    typename T::template ref_t<T>;
    typename T::template result_t<T&>;
    typename T::template result_t<const T&>;
    typename T::template result_t<T>;
    { t.next() } -> std::same_as<typename T::template result_t<T&>>;
    { std::declval<T>().next() } -> std::same_as<typename T::template result_t<T>>;
};

template <typename T, typename TValue>
concept can_push_back = std::ranges::range<T> && requires(T rng, TValue val) { rng.push_back(val); };

template <typename T, typename TValue>
concept can_insert = std::ranges::range<T> && requires(T rng, TValue val) { rng.insert(std::ranges::end(rng), val); };

template <typename T>
concept can_reserve = std::ranges::range<T> && requires(T rng) { rng.reserve(1); };
} // namespace kissra