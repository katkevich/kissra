#pragma once
#include "kissra/concepts.hpp"
#include "kissra/impl/export.hpp"
#include "kissra/type_traits.hpp"

#ifndef KISSRA_MODULE
#include <cstddef>
#include <type_traits>
#endif

KISSRA_EXPORT()
namespace kissra {
template <typename Tag>
struct ssize_mixin {
    template <kissra::mut TSelf>
        requires is_sized_v<TSelf>
    constexpr auto ssize(this TSelf&& self) {
        return std::make_signed_t<std::size_t>(self.size());
    }
};
} // namespace kissra