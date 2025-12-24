#pragma once
#include "kissra/impl/export.hpp"

#ifndef KISSRA_MODULE
#endif

KISSRA_EXPORT()
namespace kissra {
template <typename TRng>
struct is_monotonic {
    static constexpr bool value = true;
};

template <typename TRng>
constexpr bool is_monotonic_v = is_monotonic<TRng>::value;
} // namespace kissra