#pragma once
#include "kissra/concepts.hpp"

namespace kissra {
template <typename Tag>
struct front_mixin {
    template <kissra::mut TSelf>
    constexpr auto front(this TSelf&& self) {
        return self.nth(0);
    }
};
} // namespace kissra