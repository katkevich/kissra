#pragma once
#include "kissra/misc/functional.hpp"
#include "kissra/misc/type_list.hpp"
#include <concepts>
#include <ranges>
#include <utility>

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

template <typename T>
concept tuple_like = requires { std::tuple_size<std::remove_cvref_t<T>>::value; };

template <typename TFn, typename TTypeList>
concept invocable_with_type_list = is_invocable_with_type_list_v<TFn, TTypeList>;

/**
 * Destructure `TArg` (like so: auto&& [...xs] = ...) and check invocability of `TFn` with those `...xs`.
 * Basically `TArg` should be either an aggregate or implement tuple protocol (`std::tuple_size<TArg>`)
 */
template <typename TFn, typename TArg>
concept invocable_with_destructured_arg = (std::is_aggregate_v<std::remove_cvref_t<TArg>> || tuple_like<TArg>) &&
    invocable_with_type_list<TFn, destructured_members_type_list_t<TArg>>;

template <typename TFn, typename TArg>
concept regular_invocable_with_destructured_arg = invocable_with_destructured_arg<TFn, TArg>;

template <typename TFn, typename... TArgs>
concept invocable =
    std::invocable<TFn, TArgs...> || (sizeof...(TArgs) == 1 && invocable_with_destructured_arg<TFn, tmp::front_t<TArgs...>>);

template <typename TFn, typename... TArgs>
concept regular_invocable = kissra::invocable<TFn, TArgs...>;
} // namespace kissra