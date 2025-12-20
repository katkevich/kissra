#pragma once
#include "kissra/concepts.hpp"
#include "kissra/impl/export.hpp"
#include "kissra/type_traits.hpp"

#ifndef KISSRA_MODULE
#endif

KISSRA_EXPORT()
namespace kissra {
template <typename Tag>
struct back_mixin {
    template <kissra::mut TSelf>
        requires is_common_v<TSelf> && is_bidir_v<TSelf>
    constexpr auto back(this TSelf&& self) {
        return self.nth_back(0);
    }
};
} // namespace kissra