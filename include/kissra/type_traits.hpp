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
using iter_value_t = typename iter_value<TIt>::type;

template <typename TIt>
using iter_reference_t = typename iter_reference<TIt>::type;

template <typename TIt>
using iter_const_reference_t = typename iter_const_reference<TIt>::type;
} // namespace kissra