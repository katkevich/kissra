#pragma once
#include "kissra/concepts.hpp"

namespace kissra {
struct front_mixin {
    template <kissra::mut TSelf>
    auto front(this TSelf&& self) {
        return self.nth(0);
    }
};
} // namespace kissra