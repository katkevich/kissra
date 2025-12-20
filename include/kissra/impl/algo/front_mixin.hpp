#pragma once
#include "kissra/concepts.hpp"
#include "kissra/impl/export.hpp"

#ifndef KISSRA_MODULE
#endif

KISSRA_EXPORT()
namespace kissra {
template <typename Tag>
struct front_mixin {
    template <kissra::mut TSelf>
    constexpr auto front(this TSelf&& self) {
        return self.nth(0);
    }
};
} // namespace kissra