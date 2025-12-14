#pragma once
#include "kissra/concepts.hpp"
#include "kissra/type_traits.hpp"

namespace kissra {
template <typename Tag>
struct back_mixin {
    template <kissra::mut TSelf>
        requires is_common_v<TSelf> && is_bidir_v<TSelf>
    auto back(this TSelf&& self) {
        return self.nth_back(0);
    }
};
} // namespace kissra