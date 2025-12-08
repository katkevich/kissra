#pragma once
#include <type_traits>

namespace kissra {
template <typename TIt>
struct iter_value {
    using type = typename std::remove_reference_t<TIt>::value_type;
};

template <typename TIt>
struct iter_reference {
    using type = typename std::remove_reference_t<TIt>::reference;
};

template <typename TIt>
struct iter_const_reference {
    using type = typename std::remove_reference_t<TIt>::const_reference;
};

template <typename TIt>
struct iter_result {
    using type = typename std::remove_reference_t<TIt>::template result_t<TIt>;
};

template <typename TIt>
using iter_value_t = typename iter_value<TIt>::type;

template <typename TIt>
using iter_reference_t = typename iter_reference<TIt>::type;

template <typename TIt>
using iter_const_reference_t = typename iter_const_reference<TIt>::type;

template <typename TIt>
using iter_result_t = typename iter_result<TIt>::type;

template <typename T>
constexpr bool is_sized_v = std::remove_cvref_t<T>::is_sized;

template <typename T>
constexpr bool is_common_v = std::remove_cvref_t<T>::is_common;

template <typename T>
constexpr bool is_forward_v = std::remove_cvref_t<T>::is_forward;

template <typename T>
constexpr bool is_bidir_v = std::remove_cvref_t<T>::is_bidir;

template <typename T>
constexpr bool is_random_v = std::remove_cvref_t<T>::is_random;
} // namespace kissra